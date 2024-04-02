#include "imperator/module/gfx/gfx_context.h"

namespace imp {
GfxContext::GfxContext(ModuleMgr& module_mgr, GfxParams params)
  : Module(module_mgr),
    gl(GladGLContext()) {
  window = module_mgr_.get<Window>();

  glfwMakeContextCurrent(window->handle());

  auto glad_version = gladLoadGLContext(&gl, glfwGetProcAddress);
  if (glad_version == 0) {
    IMPERATOR_LOG_CRITICAL("Failed to initialize OpenGL");
    std::exit(EXIT_FAILURE);
  }

  glm::ivec2 version;
  version.x = GLAD_VERSION_MAJOR(glad_version);
  version.y = GLAD_VERSION_MINOR(glad_version);
  IMPERATOR_LOG_DEBUG("Initialized OpenGL");
  IMPERATOR_LOG_DEBUG("=> Version: {}", (char *)gl.GetString(GL_VERSION));
  IMPERATOR_LOG_DEBUG("=> Vendor: {}", (char *)gl.GetString(GL_VENDOR));
  IMPERATOR_LOG_DEBUG("=> Renderer: {}", (char *)gl.GetString(GL_RENDERER));

  if (version.x != params.backend_version.x || version.y != params.backend_version.y)
    IMPERATOR_LOG_WARN("Requested OpenGL v{}.{}", params.backend_version.x, params.backend_version.y);

#if !defined(NDEBUG)
  gl.Enable(GL_DEBUG_OUTPUT);
  gl.DebugMessageCallback(gl_message_callback_, nullptr);
#endif
}

void GfxContext::gl_message_callback_(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) {
#define STRINGIFY(e) case e: return #e;
  const std::string source_str = std::invoke(
    [source] {
      switch (source) {
      STRINGIFY(GL_DEBUG_SOURCE_API)
      STRINGIFY(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
      STRINGIFY(GL_DEBUG_SOURCE_SHADER_COMPILER)
      STRINGIFY(GL_DEBUG_SOURCE_THIRD_PARTY)
      STRINGIFY(GL_DEBUG_SOURCE_APPLICATION)
      STRINGIFY(GL_DEBUG_SOURCE_OTHER)
      default: return "?";
      }
    }
  );

  const std::string type_str = std::invoke(
    [type] {
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
    }
  );

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    IMPERATOR_LOG_ERROR(
      "OpenGL: source={} type={} id={} msg={}",
      source_str.substr(9),
      type_str.substr(9),
      id,
      message
    );
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    IMPERATOR_LOG_WARN(
      "OpenGL: source={} type={} id={} msg={}",
      source_str.substr(9),
      type_str.substr(9),
      id,
      message
    );
    break;
  case GL_DEBUG_SEVERITY_LOW:
    IMPERATOR_LOG_DEBUG(
      "OpenGL: source={} type={} id={} msg={}",
      source_str.substr(9),
      type_str.substr(9),
      id,
      message
    );
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    IMPERATOR_LOG_TRACE(
      "OpenGL: source={} type={} id={} msg={}",
      source_str.substr(9),
      type_str.substr(9),
      id,
      message
    );
    break;
  default:
    break; // won't happen
  }
#undef STRINGIFY
}
} // namespace imp
