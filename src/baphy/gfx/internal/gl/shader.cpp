#include "baphy/gfx/internal/gl/shader.hpp"

#include "baphy/util/rnd.hpp"
#include "baphy/util/sops.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <utility>

namespace baphy {

std::optional<std::string> read_file_process_includes(
    const std::filesystem::path &path,
    std::vector<std::filesystem::path> &included_paths
) {
  const static RE2 spaces_pat{R"((\s+))"};
  assert(spaces_pat.ok());

  const static RE2 path_pat{R"("(.+)\")"};
  assert(path_pat.ok());

  included_paths.emplace_back(path);

  std::ifstream ifs(path.string());
  if (!ifs.is_open()) {
    BAPHY_LOG_ERROR("Failed to open file: '{}'", path.string());
    return std::nullopt;
  }

  std::string s;
  std::size_t line_no = 1;
  for (std::string line; std::getline(ifs, line); ++line_no) {
    rtrim(line);

    if (line.starts_with("#include")) {
      auto tokens = split(line, spaces_pat);

      std::string include;
      if (tokens.size() < 2 || !RE2::FullMatch(tokens[1], path_pat, &include)) {
        BAPHY_LOG_ERROR("Failed to parse include in file {}:{}: '{}'", path.string(), line_no, line);
        return std::nullopt;
      }

      auto include_path = path.parent_path() / include;
      if (std::ranges::find(included_paths, include_path) != included_paths.end())
        continue;  // Don't try and include a file twice

      auto include_file = read_file_process_includes(include_path, included_paths);
      if (!include_file)
        return std::nullopt;

      s += *include_file + '\n';

    } else
      s += line + '\n';
  }

  return s;
}

std::optional<ShaderSrc> try_parse_shader_src(const std::string &src) {
  enum class BufferDst {vertex, fragment};

  // Match things of the form
  //   #pragma <name>(<args>)<bad>\n
  // Captures what is after the parens to emit warnings
  const static RE2 pragma_w_args(R"(#pragma (.+)\((.+)\)(.*))");
  assert(pragma_w_args.ok());

  // Match things of the form
  //   #pragma <name>\n
  // Should reject parens
  const static RE2 pragma_no_args(R"(#pragma ([^()\n]+))");
  assert(pragma_no_args.ok());

  ShaderSrc s{};
  // Intermediate storage control
  std::string buffer;
  bool read_non_pragma_line = false;
  BufferDst buffer_dst{BufferDst::vertex};

  auto try_set_shader_part = [&](){
    if (!read_non_pragma_line) {
      buffer.clear();
      return;
    }

    trim(buffer);
    buffer += "\n";

    switch (buffer_dst) {
      using enum BufferDst;
      case vertex:
        if (!s.vertex)
          s.vertex = buffer;
        break;

      case fragment:
        if (!s.fragment)
          s.fragment = buffer;
        break;
    }

    buffer.clear();
  };

  // Variables to store regex match captures
  std::string pragma_name;
  std::string pragma_args;
  std::string pragma_extra;

  std::istringstream iss{src};
  std::size_t line_no = 1;
  for (std::string line; std::getline(iss, line); ++line_no) {
    rtrim(line);

    if (line.empty()) {
      if (!buffer.empty())
        buffer.append("\n");
      continue;
    }

    if (RE2::FullMatch(line, pragma_w_args, &pragma_name, &pragma_args, &pragma_extra)) {
      if (pragma_name == "name")
        s.name = pragma_args;
      else
        BAPHY_LOG_WARN("Unrecognized pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

      if (!pragma_extra.empty())
        BAPHY_LOG_WARN("Trailing characters on pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

    } else if (RE2::FullMatch(line, pragma_no_args, &pragma_name)) {
      if (pragma_name == "vertex") {
        if (s.vertex)
          BAPHY_LOG_WARN("Duplicate vertex pragma in shader {}:{}, will ignore", s.name.value_or("undef"), line_no);
        try_set_shader_part();
        buffer_dst = BufferDst::vertex;

      } else if (pragma_name == "fragment") {
        if (s.fragment)
          BAPHY_LOG_WARN("Duplicate fragment pragma in shader {}:{}, will ignore", s.name.value_or("undef"), line_no);
        try_set_shader_part();
        buffer_dst = BufferDst::fragment;

      } else
        BAPHY_LOG_WARN("Unrecognized pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

    } else {
      read_non_pragma_line = true;
      buffer.append(line + '\n');
    }
  }

  if (!buffer.empty())
    try_set_shader_part();

  return s;
}

std::string ShaderSrc::get() const {
  std::string s;

  if (name) {
    s += fmt::format("#pragma name({})\n", *name);
    s += "\n";
  }

  if (vertex) {
    s += fmt::format("#pragma vertex\n");
    s += "\n";
    s += fmt::format("{}\n", *vertex);
  }

  if (fragment) {
    s += fmt::format("#pragma fragment\n");
    s += "\n";
    s += fmt::format("{}\n", *fragment);
  }

  return s;
}

std::optional<ShaderSrc> ShaderSrc::parse(const std::string &src) {
  return try_parse_shader_src(src);
}

std::optional<ShaderSrc> ShaderSrc::parse(const std::filesystem::path &path) {
  std::vector<std::filesystem::path> included_paths{};
  auto src = read_file_process_includes(path, included_paths);
  if (!src)
    return std::nullopt;

  return try_parse_shader_src(*src);
}

Shader::Shader(std::shared_ptr<GfxContext> gfx, const ShaderSrc &src)
    : gfx(std::move(gfx)), name(src.name.value_or(base58(11))) {
  gen_id_();
  compile_shader_src_(src);
}

Shader::~Shader() {
  del_id_();
}

std::string Shader::src() const {
  return src_.get();
}

void Shader::recompile(const ShaderSrc &src) {
  auto old_id = id;

  gen_id_();
  if (compile_shader_src_(src))
    del_id_(old_id);
  else
    id = old_id;
}

void Shader::use() {
  gfx->gl->UseProgram(id);
}

void Shader::uniform_1f(const std::string &uniform_name, float v0) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform1f(loc, v0);
}

void Shader::uniform_2f(const std::string &uniform_name, float v0, float v1) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform2f(loc, v0, v1);
}

void Shader::uniform_3f(const std::string &uniform_name, float v0, float v1, float v2) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform3f(loc, v0, v1, v2);
}

void Shader::uniform_4f(const std::string &uniform_name, float v0, float v1, float v2, float v3) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform4f(loc, v0, v1, v2, v3);
}

void Shader::uniform_1f(const std::string &uniform_name, glm::vec1 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform1fv(loc, 1, &v.x);
}

void Shader::uniform_2f(const std::string &uniform_name, glm::vec2 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform2fv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3f(const std::string &uniform_name, glm::vec3 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform3fv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4f(const std::string &uniform_name, glm::vec4 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform4fv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_1i(const std::string &uniform_name, int v0) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform1i(loc, v0);
}

void Shader::uniform_2i(const std::string &uniform_name, int v0, int v1) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform2i(loc, v0, v1);
}

void Shader::uniform_3i(const std::string &uniform_name, int v0, int v1, int v2) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform3i(loc, v0, v1, v2);
}

void Shader::uniform_4i(const std::string &uniform_name, int v0, int v1, int v2, int v3) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform4i(loc, v0, v1, v2, v3);
}

void Shader::uniform_1i(const std::string &uniform_name, glm::ivec1 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform1iv(loc, 1, &v.x);
}

void Shader::uniform_2i(const std::string &uniform_name, glm::ivec2 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform2iv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3i(const std::string &uniform_name, glm::ivec3 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform3iv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4i(const std::string &uniform_name, glm::ivec4 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform4iv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_1u(const std::string &uniform_name, unsigned int v0) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform1ui(loc, v0);
}

void Shader::uniform_2u(const std::string &uniform_name, unsigned int v0, unsigned int v1) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform2ui(loc, v0, v1);
}

void Shader::uniform_3u(const std::string &uniform_name, unsigned int v0, unsigned int v1, unsigned int v2) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform3ui(loc, v0, v1, v2);
}

void Shader::uniform_4u(const std::string &uniform_name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform4ui(loc, v0, v1, v2, v3);
}

void Shader::uniform_1u(const std::string &uniform_name, glm::uvec1 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform1uiv(loc, 1, &v.x);
}

void Shader::uniform_2u(const std::string &uniform_name, glm::uvec2 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform2uiv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3u(const std::string &uniform_name, glm::uvec3 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform3uiv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4u(const std::string &uniform_name, glm::uvec4 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->Uniform4uiv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_mat2f(const std::string &uniform_name, glm::mat2 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3f(const std::string &uniform_name, glm::mat3 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4f(const std::string &uniform_name, glm::mat4 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2x3f(const std::string &uniform_name, glm::mat2x3 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix2x3fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3x2f(const std::string &uniform_name, glm::mat3x2 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix3x2fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2x4f(const std::string &uniform_name, glm::mat2x4 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix2x4fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4x2f(const std::string &uniform_name, glm::mat4x2 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix4x2fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3x4f(const std::string &uniform_name, glm::mat3x4 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix3x4fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4x3f(const std::string &uniform_name, glm::mat4x3 v) {
  auto loc = get_uniform_loc_(uniform_name);
  if (loc != -1)
    gfx->gl->UniformMatrix4x3fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

GLint Shader::get_uniform_loc_(const std::string &uniform_name) {
  auto it = uniform_locs_.find(uniform_name);
  if (it != uniform_locs_.end())
    return it->second;

  GLint loc = gfx->gl->GetUniformLocation(id, uniform_name.c_str());
  if (loc == -1)
    BAPHY_LOG_WARN("Uniform '{}' not found in shader ({}:{})", uniform_name, name, id);
  uniform_locs_[uniform_name] = loc;

  return loc;
}

bool Shader::compile_shader_src_(const ShaderSrc &src) {
  GLuint vertex_id{0};
  GLuint fragment_id{0};

  if (src.vertex) {
    vertex_id = gfx->gl->CreateShader(GL_VERTEX_SHADER);

    const char *src_p = src.vertex.value().c_str();
    gfx->gl->ShaderSource(vertex_id, 1, &src_p, nullptr);
    gfx->gl->CompileShader(vertex_id);

    if (check_compile_(vertex_id, GL_VERTEX_SHADER)) {
      gfx->gl->AttachShader(id, vertex_id);
      BAPHY_LOG_DEBUG("Attached vertex shader ({}:{})", name, id);
    } else
      return false;
  }

  if (src.fragment) {
    fragment_id = gfx->gl->CreateShader(GL_FRAGMENT_SHADER);

    const char *src_p = src.fragment.value().c_str();
    gfx->gl->ShaderSource(fragment_id, 1, &src_p, nullptr);
    gfx->gl->CompileShader(fragment_id);

    if (check_compile_(fragment_id, GL_FRAGMENT_SHADER)) {
      gfx->gl->AttachShader(id, fragment_id);
      BAPHY_LOG_DEBUG("Attached fragment shader ({}:{})", name, id);
    } else
      return false;
  }

  gfx->gl->LinkProgram(id);
  if (check_link_()) {
    BAPHY_LOG_DEBUG("Linked shader program ({}:{})", name, id);
    src_ = src;
  } else
    return false;

  if (vertex_id != 0)
    gfx->gl->DeleteShader(vertex_id);

  if (fragment_id != 0)
    gfx->gl->DeleteShader(fragment_id);

  return true;
}

bool Shader::check_compile_(GLuint shader_id, GLenum type) {
  static auto info_log = std::vector<char>();
  static int success;

  gfx->gl->GetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    gfx->gl->GetShaderInfoLog(shader_id, static_cast<GLsizei>(info_log.size()), nullptr, &info_log[0]);

    std::string type_str;
    switch (type) {
      case GL_VERTEX_SHADER:
        type_str = "vertex";
        break;
      case GL_FRAGMENT_SHADER:
        type_str = "fragment";
        break;
      default:
        std::unreachable();
    }

    BAPHY_LOG_ERROR("Failed to compile {} shader ({}:{})! Info log:\n{}", type_str, name, id, &info_log[0]);
    return false;
  }

  return true;
}

bool Shader::check_link_() {
  static auto info_log = std::vector<char>();
  static int success;

  gfx->gl->GetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    gfx->gl->GetProgramInfoLog(id, static_cast<GLsizei>(info_log.size()), nullptr, &info_log[0]);

    BAPHY_LOG_ERROR("Failed to link shader program ({}:{})! Info log:\n{}", name, id, &info_log[0]);
    return false;
  }

  return true;
}

void Shader::gen_id_() {
  id = gfx->gl->CreateProgram();
  BAPHY_LOG_DEBUG("GEN_ID({}): Shader/{}", id, name);
}

void Shader::del_id_(GLuint id) {
  if (id != 0) {
    gfx->gl->DeleteProgram(id);
    BAPHY_LOG_DEBUG("DEL_ID({}): Shader/{}", id, name);
  }
}

void Shader::del_id_() {
  del_id_(id);
}

} // namespace baphy
