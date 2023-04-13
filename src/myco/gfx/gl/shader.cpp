#include "myco/gfx/gl/shader.hpp"
#include "myco/util/io.hpp"
#include "myco/util/sops.hpp"
#include "re2/re2.h"
#include <sstream>

namespace myco::gl {

ShaderSrc ShaderSrc::parse_src(const std::string &src) {
  enum class BufferDst {vertex, fragment};

  // Match things of the form
  //   #pragma <name>(<args>)<bad>\n
  // Captures what is after the parens to emit warnings
  static RE2 pragma_w_args(R"(#pragma (.+)\((.+)\)(.*))");
  assert(pragma_w_args.ok());

  // Match things of the form
  //   #pragma <name>\n
  // Should reject parens
  static RE2 pragma_no_args(R"(#pragma ([^()\n]+))");
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

  std::istringstream iss(src);
  std::size_t line_no = 1;
  for (std::string line; std::getline(iss, line); ++line_no) {
    myco::rtrim(line);
    if (line.empty())
      continue;

    if (RE2::FullMatch(line, pragma_w_args, &pragma_name, &pragma_args, &pragma_extra)) {
      if (pragma_name == "name")
        s.name = pragma_args;

      else
        MYCO_LOG_WARN("Unrecognized pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

      if (!pragma_extra.empty())
        MYCO_LOG_WARN("Trailing characters on pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

    } else if (RE2::FullMatch(line, pragma_no_args, &pragma_name)) {
      if (pragma_name == "vertex") {
        if (s.vertex)
          MYCO_LOG_WARN("Duplicate vertex pragma in shader {}:{}, will ignore", s.name.value_or("undef"), line_no);
        try_set_shader_part();
        buffer_dst = BufferDst::vertex;

      } else if (pragma_name == "fragment") {
        if (s.fragment)
          MYCO_LOG_WARN("Duplicate fragment pragma in shader {}:{}, will ignore", s.name.value_or("undef"), line_no);
        try_set_shader_part();
        buffer_dst = BufferDst::fragment;

      } else
        MYCO_LOG_WARN("Unrecognized pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

    } else {
      read_non_pragma_line = true;
      buffer.append(line + '\n');
    }
  }

  if (!buffer.empty())
    try_set_shader_part();

  return s;
}

ShaderSrc ShaderSrc::parse_file(const std::filesystem::path &path) {
  return parse_src(read_file_to_string(path));
}

Shader::~Shader() {
  del_id_();
}

Shader::Shader(std::shared_ptr<Context2D> ctx, const myco::gl::ShaderSrc &src)
    : name(src.name.value_or(base58(11))), ctx_(std::move(ctx)) {
  gen_id_();

  GLuint vertex_id{0};
  GLuint fragment_id{0};

  if (src.vertex) {
    vertex_id = ctx_->gl->CreateShader(GL_VERTEX_SHADER);

    const char *src_p = src.vertex.value().c_str();
    ctx_->gl->ShaderSource(vertex_id, 1, &src_p, nullptr);
    ctx_->gl->CompileShader(vertex_id);

    if (check_compile_(vertex_id, GL_VERTEX_SHADER)) {
      ctx_->gl->AttachShader(id, vertex_id);
      MYCO_LOG_DEBUG("Attached vertex shader ({}:{})", name, id);
    }
  }

  if (src.fragment) {
    fragment_id = ctx_->gl->CreateShader(GL_FRAGMENT_SHADER);

    const char *src_p = src.fragment.value().c_str();
    ctx_->gl->ShaderSource(fragment_id, 1, &src_p, nullptr);
    ctx_->gl->CompileShader(fragment_id);

    if (check_compile_(fragment_id, GL_FRAGMENT_SHADER)) {
      ctx_->gl->AttachShader(id, fragment_id);
      MYCO_LOG_DEBUG("Attached fragment shader ({}:{})", name, id);
    }
  }

  ctx_->gl->LinkProgram(id);
  if (check_link_())
    MYCO_LOG_DEBUG("Linked shader program ({}:{})", name, id);

  if (vertex_id != 0)
    ctx_->gl->DeleteShader(vertex_id);

  if (fragment_id != 0)
    ctx_->gl->DeleteShader(fragment_id);
}

bool Shader::check_compile_(GLuint shader_id, GLenum type) {
  static auto info_log = std::vector<char>();
  static int success;

  ctx_->gl->GetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    ctx_->gl->GetShaderInfoLog(shader_id, static_cast<GLsizei>(info_log.size()), nullptr, &info_log[0]);

    std::string type_str;
    switch (type) {
      case GL_VERTEX_SHADER:
        type_str = "vertex";
        break;
      case GL_FRAGMENT_SHADER:
        type_str = "fragment";
        break;
    }

    MYCO_LOG_ERROR("Failed to compile {} shader ({}:{})! Info log:\n{}", type_str, name, id, &info_log[0]);
    return false;
  }

  return true;
}

bool Shader::check_link_() {
  static auto info_log = std::vector<char>();
  static int success;

  ctx_->gl->GetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    ctx_->gl->GetProgramInfoLog(id, static_cast<GLsizei>(info_log.size()), nullptr, &info_log[0]);

    MYCO_LOG_ERROR("Failed to link shader program ({}:{})! Info log:\n{}", name, id, &info_log[0]);
    return false;
  }

  return true;
}

void Shader::gen_id_() {
  id = ctx_->gl->CreateProgram();
  MYCO_LOG_DEBUG("GEN_ID({}): Shader/{}", id, name);
}

void Shader::del_id_() {
  if (id != 0) {
    ctx_->gl->DeleteProgram(id);
    MYCO_LOG_DEBUG("DEL_ID({}): Shader/{}", id, name);
  }
}

} // namespace myco::gl
