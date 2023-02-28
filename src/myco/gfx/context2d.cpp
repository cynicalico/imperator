#include "myco/gfx/context2d.hpp"

namespace myco {

Context2D::Context2D(const Window &window) {
  gl = std::make_unique<GladGLContext>();
  auto glad_version = gladLoadGLContext(gl.get(), glfwGetProcAddress);
  if (glad_version == 0) {
    MYCO_LOG_CRITICAL("Failed to initialize OpenGL");
    std::exit(EXIT_FAILURE);
  }

  auto glad_major = GLAD_VERSION_MAJOR(glad_version);
  auto glad_minor = GLAD_VERSION_MINOR(glad_version);
  MYCO_LOG_DEBUG("Initialized OpenGL v{}.{}", glad_major, glad_minor);

  WindowOpenParams open_params = window.get_open_params();
  if (glad_major != open_params.backend_version.x || glad_minor != open_params.backend_version.y)
    MYCO_LOG_WARN("Requested OpenGL v{}.{}", open_params.backend_version.x, open_params.backend_version.y);

  gl->Enable(GL_DEBUG_OUTPUT);
  gl->DebugMessageCallback(gl_message_callback_, nullptr);
}

void Context2D::clear(const RGB &color, const ClearBit &bit) {
  gl->ClearColor(
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f);
  gl->Clear(unwrap(bit));
}

void GLAPIENTRY Context2D::gl_message_callback_(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei,
    const GLchar *message,
    const void *
) {
#define STRINGIFY(e) case e: return #e;
  std::string source_str = ([source](){
    switch (source) {
      STRINGIFY(GL_DEBUG_SOURCE_API)
      STRINGIFY(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
      STRINGIFY(GL_DEBUG_SOURCE_SHADER_COMPILER)
      STRINGIFY(GL_DEBUG_SOURCE_THIRD_PARTY)
      STRINGIFY(GL_DEBUG_SOURCE_APPLICATION)
      STRINGIFY(GL_DEBUG_SOURCE_OTHER)
      default: return "?";
    }
  })();

  std::string type_str = ([type](){
    switch (type) {
      STRINGIFY(GL_DEBUG_TYPE_ERROR)
      STRINGIFY(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
      STRINGIFY(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
      STRINGIFY(GL_DEBUG_TYPE_PORTABILITY)
      STRINGIFY(GL_DEBUG_TYPE_PERFORMANCE)
      STRINGIFY(GL_DEBUG_TYPE_MARKER)
      STRINGIFY(GL_DEBUG_TYPE_PUSH_GROUP)
      STRINGIFY(GL_DEBUG_TYPE_POP_GROUP)
      STRINGIFY(GL_DEBUG_TYPE_OTHER)
      default: return "?";
    }
  })();

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      MYCO_LOG_ERROR("OpenGL: source={} type={} id={} msg={}", source_str, type_str, id, message);
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      MYCO_LOG_WARN("OpenGL: source={} type={} id={} msg={}", source_str, type_str, id, message);
      break;
    case GL_DEBUG_SEVERITY_LOW:
      MYCO_LOG_DEBUG("OpenGL: source={} type={} id={} msg={}", source_str, type_str, id, message);
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      MYCO_LOG_TRACE("OpenGL: source={} type={} id={} msg={}", source_str, type_str, id, message);
      break;
    default:
      break; // won't happen
  }
#undef STRINGIFY
}

} // namespace myco
