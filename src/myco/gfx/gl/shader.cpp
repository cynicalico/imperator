#include "myco/gfx/gl/shader.hpp"

namespace myco::gl {

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
