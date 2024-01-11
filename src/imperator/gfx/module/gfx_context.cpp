#include "imperator/gfx/module/gfx_context.hpp"

#define GLFW_INCLUDE_NONE
#include "imperator/util/log.hpp"
#include "imperator/util/platform.hpp"
#include "imgui.h"
#if defined(IMPERATOR_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include "GLFW/glfw3native.h"
#elif defined(IMPERATOR_PLATFORM_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_NATIVE_INCLUDE_NONE
#include "GLFW/glfw3native.h"
#include "glad/glx.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

namespace imp {
bool GfxContext::is_vsync() const {
  return platform_is_vsync_();
}

void GfxContext::set_vsync(bool v) {
  platform_set_vsync_(v);
}

void GfxContext::clear(const Color& color, const ClearBit& mask) {
  auto gl_color = color.gl_color();
  gl.ClearColor(gl_color.r, gl_color.g, gl_color.b, gl_color.a);
  gl.Clear(unwrap(mask));
}

void GfxContext::r_initialize_(const E_Initialize& p) {
  debug_overlay = module_mgr->get<DebugOverlay>();
  window = module_mgr->get<Window>();

  glfwMakeContextCurrent(window->handle());

  gl = GladGLContext();
  auto glad_version = gladLoadGLContext(&gl, glfwGetProcAddress);
  if (glad_version == 0) {
    IMP_LOG_CRITICAL("Failed to initialize OpenGL");
    std::exit(EXIT_FAILURE);
  }

  version.x = GLAD_VERSION_MAJOR(glad_version);
  version.y = GLAD_VERSION_MINOR(glad_version);
  IMP_LOG_DEBUG("Initialized OpenGL");
  IMP_LOG_DEBUG("=> Version: {}", (char *)gl.GetString(GL_VERSION));
  IMP_LOG_DEBUG("=> Vendor: {}", (char *)gl.GetString(GL_VENDOR));
  IMP_LOG_DEBUG("=> Renderer: {}", (char *)gl.GetString(GL_RENDERER));

  if (version.x != p.params.backend_version.x || version.y != p.params.backend_version.y)
    IMP_LOG_WARN("Requested OpenGL v{}.{}", p.params.backend_version.x, p.params.backend_version.y);

  initialize_platform_extensions_();

  // We set to false and then true because wglGetSwapInterval doesn't
  // seem to return the correct value on an initial call unless we have
  // explicitly set something beforehand
  set_vsync(false);
  if (is_flag_set(p.params.flags, WindowFlags::vsync))
    set_vsync(true);

#if !defined(NDEBUG)
  gl.Enable(GL_DEBUG_OUTPUT);
  gl.DebugMessageCallback(gl_message_callback_, nullptr);
#endif

  debug_overlay->add_tab(module_name, [&] {
    ImGui::Separator();

    if (bool v = is_vsync(); ImGui::Checkbox("Vsync", &v)) {
      set_vsync(v);
    }
  });

  Module::r_initialize_(p);
}

void GfxContext::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
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
  const std::string source_str = std::invoke([source] {
    switch (source) {
      STRINGIFY(GL_DEBUG_SOURCE_API)
      STRINGIFY(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
      STRINGIFY(GL_DEBUG_SOURCE_SHADER_COMPILER)
      STRINGIFY(GL_DEBUG_SOURCE_THIRD_PARTY)
      STRINGIFY(GL_DEBUG_SOURCE_APPLICATION)
      STRINGIFY(GL_DEBUG_SOURCE_OTHER)
      default: return "?";
    }
  });

  const std::string type_str = std::invoke([type] {
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
  });

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      IMP_LOG_ERROR("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      IMP_LOG_WARN("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    case GL_DEBUG_SEVERITY_LOW:
      IMP_LOG_DEBUG("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      IMP_LOG_TRACE("OpenGL: source={} type={} id={} msg={}", source_str.substr(9), type_str.substr(9), id, message);
      break;
    default:
      break; // won't happen
  }
#undef STRINGIFY
}

#if defined(IMPERATOR_PLATFORM_WINDOWS)
void GfxContext::initialize_platform_extensions_() {
  HDC dc = GetDC(glfwGetWin32Window(window->handle()));
  auto wgl_version = gladLoadWGL(dc, glfwGetProcAddress);
  if (wgl_version == 0) {
    IMP_LOG_WARN("Failed to initialize WGL");
    return;
  }

  auto version_major = GLAD_VERSION_MAJOR(wgl_version);
  auto version_minor = GLAD_VERSION_MINOR(wgl_version);
  IMP_LOG_DEBUG("Initialized WGL v{}.{}", version_major, version_minor);

  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  if (!wglSwapIntervalEXT)
    IMP_LOG_WARN("Failed to load extension wglSwapIntervalEXT");

  wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
  if (!wglGetSwapIntervalEXT)
    IMP_LOG_WARN("Failed to load extension wglGetSwapIntervalEXT");
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
    IMP_LOG_CRITICAL("Failed to initialize GLX");
    std::exit(EXIT_FAILURE);
  }

  auto version_major = GLAD_VERSION_MAJOR(glx_version);
  auto version_minor = GLAD_VERSION_MINOR(glx_version);
  IMP_LOG_DEBUG("Initialized GLX v{}.{}", version_major, version_minor);
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
} // namespace imp
