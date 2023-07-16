#include "baphy/gfx/module/gfx_context.hpp"

#define GLFW_INCLUDE_NONE
#include "baphy/util/log.hpp"
#include "baphy/util/platform.hpp"
#include "GLFW/glfw3.h"
#if defined(BAPHY_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include "GLFW/glfw3native.h"
#elif defined(BAPHY_PLATFORM_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_NATIVE_INCLUDE_NONE
#include "GLFW/glfw3native.h"
#endif
#include "glm/gtx/transform.hpp"

namespace baphy {

bool GfxContext::is_vsync() const {
  return platform_is_vsync_();
}

void GfxContext::set_vsync(bool v) {
  platform_set_vsync_(v);
}

void GfxContext::enable(Capability c) {
  gl.Enable(unwrap(c));
}

void GfxContext::disable(Capability c) {
  gl.Disable(unwrap(c));
}

void GfxContext::clear(const RGB &color, const ClearBit &bit) {
  gl.ClearColor(
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f);
  gl.Clear(unwrap(bit));
}

void GfxContext::blend_func(BlendFunc s_factor, BlendFunc d_factor) {
  gl.BlendFunc(unwrap(s_factor), unwrap(d_factor));
}

void GfxContext::blend_func_separate(BlendFunc src_rgb, BlendFunc dst_rgb, BlendFunc src_alpha, BlendFunc dst_alpha) {
  gl.BlendFuncSeparate(unwrap(src_rgb), unwrap(dst_rgb), unwrap(src_alpha), unwrap(dst_alpha));
}

void GfxContext::depth_mask(bool enabled) {
  gl.DepthMask(enabled ? 1 : 0);
}

void GfxContext::flush_draw_calls() {
  EventBus::send_nowait<EFlush>();
}

void GfxContext::flush() {
  gl.Flush();
}

glm::mat4 GfxContext::ortho_projection() const {
  return glm::ortho(
      0.0f, static_cast<float>(window->w()),
      static_cast<float>(window->h()), 0.0f,
      0.0f, 1.0f);
}

#if defined(BAPHY_PLATFORM_WINDOWS)
void GfxContext::initialize_platform_extensions_() {
  HDC dc = GetDC(glfwGetWin32Window(window->handle()));
  auto wgl_version = gladLoadWGL(dc, glfwGetProcAddress);
  if (wgl_version == 0) {
    BAPHY_LOG_WARN("Failed to initialize WGL");
    return;
  }

  auto version_major = GLAD_VERSION_MAJOR(wgl_version);
  auto version_minor = GLAD_VERSION_MINOR(wgl_version);
  BAPHY_LOG_DEBUG("Initialized WGL v{}.{}", version_major, version_minor);

  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  if (!wglSwapIntervalEXT)
    BAPHY_LOG_WARN("Failed to load extension wglSwapIntervalEXT");

  wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
  if (!wglGetSwapIntervalEXT)
    BAPHY_LOG_WARN("Failed to load extension wglGetSwapIntervalEXT");
}

bool GfxContext::platform_is_vsync_() const {
  if (wglGetSwapIntervalEXT)
    return wglGetSwapIntervalEXT() == 1;
  return false;
}

void GfxContext::platform_set_vsync_(bool v) {
  if (wglSwapIntervalEXT)
    wglSwapIntervalEXT(v ? 1 : 0);
  else
    glfwSwapInterval(v ? 1 : 0);
}

#else
// Unsure what this will do in a multi-monitor setup if the
// monitors are actually recognized as separate screens

void GfxContext::initialize_platform_extensions_() {
  Display *dpy = glfwGetX11Display();
  int screen = DefaultScreen(dpy);
  auto glx_version = gladLoadGLX(dpy, screen, glfwGetProcAddress);
  if (glx_version == 0) {
    BAPHY_LOG_CRITICAL("Failed to initialize GLX");
    std::exit(EXIT_FAILURE);
  }

  auto version_major = GLAD_VERSION_MAJOR(glx_version);
  auto version_minor = GLAD_VERSION_MINOR(glx_version);
  BAPHY_LOG_DEBUG("Initialized GLX v{}.{}", version_major, version_minor);
}

bool GfxContext::platform_is_vsync_() const {
  Display *dpy = glXGetCurrentDisplay();
  GLXDrawable drawable = glXGetCurrentDrawable();

  unsigned int swap;
  glXQueryDrawable(dpy, drawable, GLX_SWAP_INTERVAL_EXT, &swap);

  return swap > 0;
}

void GfxContext::platform_set_vsync_(bool v) {
  Display *dpy = glXGetCurrentDisplay();
  GLXDrawable drawable = glXGetCurrentDrawable();

  glXSwapIntervalEXT(dpy, drawable, v ? 1 : 0);
}
#endif

void GfxContext::e_initialize_(const baphy::EInitialize &e) {
  window = module_mgr->get<Window>();
  window->make_context_current();

  gl = GladGLContext();
  auto glad_version = gladLoadGLContext(&gl, glfwGetProcAddress);
  if (glad_version == 0) {
    BAPHY_LOG_CRITICAL("Failed to initialize OpenGL");
    std::exit(EXIT_FAILURE);
  }

  version.x = GLAD_VERSION_MAJOR(glad_version);
  version.y = GLAD_VERSION_MINOR(glad_version);
  BAPHY_LOG_DEBUG("Initialized OpenGL");
  BAPHY_LOG_DEBUG("=> Version: {}",(char *)gl.GetString(GL_VERSION));
  BAPHY_LOG_DEBUG("=> Vendor: {}", (char *)gl.GetString(GL_VENDOR));
  BAPHY_LOG_DEBUG("=> Renderer: {}", (char *)gl.GetString(GL_RENDERER));

  auto open_params = window->open_params();
  if (version.x != open_params.backend_version.x || version.y != open_params.backend_version.y)
    BAPHY_LOG_WARN("Requested OpenGL v{}.{}", open_params.backend_version.x, open_params.backend_version.y);

  initialize_platform_extensions_();

  // We set to false and then true because wglGetSwapInterval doesn't
  // seem to return the correct value on an initial call unless we have
  // explicitly set something beforehand
  set_vsync(false);
  if (set(window->open_params().flags, WindowFlags::vsync))
    set_vsync(true);

#if !defined(NDEBUG)
  gl.Enable(GL_DEBUG_OUTPUT);
  gl.DebugMessageCallback(gl_message_callback_, nullptr);
#endif

  gl.Enable(GL_BLEND);
  gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  gl.Enable(GL_DEPTH_TEST);

  gl.DepthMask(GL_TRUE);

  EventBus::sub<EGlfwWindowSize>(module_name, [&](const auto &e) { e_glfw_window_size_(e); });

  Module::e_initialize_(e);
}

void GfxContext::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void GfxContext::e_glfw_window_size_(const EGlfwWindowSize &e) {
  gl.Viewport(0, 0, e.width, e.height);
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
