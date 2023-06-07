#include "baphy/gfx/module/gfx_context.hpp"

#include "baphy/util/log.hpp"
#include "baphy/util/platform.hpp"
#include "GLFW/glfw3.h"
#if defined(BAPHY_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include "GLFW/glfw3native.h"
#endif

namespace baphy {

bool GfxContext::is_vsync() const {
  return platform_is_vsync_();
}

void GfxContext::set_vsync(bool v) {
  platform_set_vsync_(v);
}

void GfxContext::clear(const RGB &color, const ClearBit &bit) {
  gl->ClearColor(
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f);
  gl->Clear(unwrap(bit));
}

#if defined(BAPHY_PLATFORM_WINDOWS)
void GfxContext::initialize_platform_extensions_() {
  auto window = module_mgr->get<Window>();

  HDC dc = GetDC(glfwGetWin32Window(window->handle()));
  auto wgl_version = gladLoadWGL(dc, glfwGetProcAddress);
  if (wgl_version == 0) {
    BAPHY_LOG_WARN("Failed to initialize WGL");
    return;
  }

  auto version_major = GLAD_VERSION_MAJOR(wgl_version);
  auto version_minor = GLAD_VERSION_MINOR(wgl_version);
  BAPHY_LOG_DEBUG("Initialized WGL");
  BAPHY_LOG_DEBUG("=> Version: {}.{}", version_major, version_minor);

  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  if (!wglSwapIntervalEXT)
    BAPHY_LOG_WARN("Failed to load extension wglSwapIntervalEXT");

  wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
  if (!wglGetSwapIntervalEXT)
    BAPHY_LOG_WARN("Failed to load extension wglGetSwapIntervalEXT");
}

bool GfxContext::platform_is_vsync_() const {
  if (wglGetSwapIntervalEXT)
    return wglGetSwapIntervalEXT();
  return false;
}

void GfxContext::platform_set_vsync_(bool v) {
  if (wglSwapIntervalEXT)
    wglSwapIntervalEXT(v ? 1 : 0);
  else
    glfwSwapInterval(v ? 1 : 0);
}

#else
void GfxContext::initialize_platform_extensions_() {}
bool GfxContext::platform_is_vsync_() const { return false; }
void GfxContext::platform_set_vsync_(bool v) { glfwSwapInterval(v ? 1 : 0); }
#endif

void GfxContext::e_initialize_(const baphy::EInitialize &e) {
  auto window = module_mgr->get<Window>();
  window->make_context_current();

  gl = std::make_unique<GladGLContext>();
  auto glad_version = gladLoadGLContext(gl.get(), glfwGetProcAddress);
  if (glad_version == 0) {
    BAPHY_LOG_CRITICAL("Failed to initialize OpenGL");
    std::exit(EXIT_FAILURE);
  }

  version.x = GLAD_VERSION_MAJOR(glad_version);
  version.y = GLAD_VERSION_MINOR(glad_version);
  BAPHY_LOG_DEBUG("Initialized OpenGL");
  BAPHY_LOG_DEBUG("=> Version: {}",(char *)gl->GetString(GL_VERSION));
  BAPHY_LOG_DEBUG("=> Vendor: {}", (char *)gl->GetString(GL_VENDOR));
  BAPHY_LOG_DEBUG("=> Renderer: {}", (char *)gl->GetString(GL_RENDERER));

  auto open_params = window->open_params();
  if (version.x != open_params.backend_version.x || version.y != open_params.backend_version.y)
    BAPHY_LOG_WARN("Requested OpenGL v{}.{}", open_params.backend_version.x, open_params.backend_version.y);

  initialize_platform_extensions_();

#if !defined(NDEBUG)
  gl->Enable(GL_DEBUG_OUTPUT);
  gl->DebugMessageCallback(gl_message_callback_, nullptr);
#endif

  gl->Enable(GL_BLEND);
  gl->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  gl->Enable(GL_DEPTH_TEST);

  Module::e_initialize_(e);
}

void GfxContext::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void GfxContext::gl_message_callback_(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam
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
      BAPHY_LOG_ERROR("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      BAPHY_LOG_WARN("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    case GL_DEBUG_SEVERITY_LOW:
      BAPHY_LOG_DEBUG("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      BAPHY_LOG_TRACE("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    default:
      break; // won't happen
  }
#undef STRINGIFY
}

} // namespace baphy
