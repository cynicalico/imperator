#include "baphy/core/module/window.hpp"

#include "baphy/core/module/application.hpp"
#include "baphy/core/module/glfw_callbacks.hpp"
#include "baphy/util/log.hpp"

namespace baphy {
std::once_flag Window::initialize_glfw_;

int Window::w() const {
  int w;
  glfwGetWindowSize(glfw_handle_, &w, nullptr);

  return w;
}

int Window::h() const {
  int h;
  glfwGetWindowSize(glfw_handle_, &h, nullptr);

  return h;
}

void Window::r_initialize_(const E_Initialize& p) {
  Hermes::sub<E_EndFrame>(module_name, {EPI<Application>::name}, [&](const auto& p) { r_end_frame_(p); });
  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });
  Hermes::sub<E_GlfwWindowClose>(module_name, [&](const auto& p) { r_glfw_window_close_(p); });

  std::call_once(initialize_glfw_, [&]() {
    register_glfw_error_callback();

    if (glfwInit() == GLFW_FALSE) {
      BAPHY_LOG_CRITICAL("Failed to initialize GLFW");
      std::exit(EXIT_FAILURE);
    }

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    BAPHY_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, revision);
  });

  open_(p.params);

  Module::r_initialize_(p);
}

void Window::r_shutdown_(const E_Shutdown& p) {
  glfwTerminate();
  BAPHY_LOG_DEBUG("Terminated GLFW");

  Module::r_shutdown_(p);
}

GLFWmonitor* Window::get_monitor_(int monitor_num) {
  int monitor_count = 0;
  const auto monitors = glfwGetMonitors(&monitor_count);

  if (monitor_num >= monitor_count) {
    BAPHY_LOG_WARN(
      "Monitor {} out of range ({} available); defaulting to primary",
      monitor_num,
      monitor_count
    );
    return monitors[0];
  }
  return monitors[monitor_num];
}

void Window::open_(const InitializeParams& params) {
  // open_params_ = params;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, params.backend_version.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, params.backend_version.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#if !defined(NDEBUG)
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (is_flag_set(params.flags, WindowFlags::fullscreen) ||
      is_flag_set(params.flags, WindowFlags::borderless))
    open_fullscreen_(params);
  else
    open_windowed_(params);

  register_glfw_callbacks(glfw_handle_);

  if (!is_flag_set(params.flags, WindowFlags::fullscreen) &&
      !is_flag_set(params.flags, WindowFlags::borderless) &&
      !is_flag_set(params.flags, WindowFlags::hidden))
    glfwShowWindow(glfw_handle_);
}

void Window::open_fullscreen_(const InitializeParams& params) {
#if defined(BAPHY_PLATFORM_WINDOWS)
  GLFWmonitor* monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (is_flag_set(params.flags, WindowFlags::borderless)) {
    // borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), nullptr, nullptr);
  } else
    glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);

  if (!glfw_handle_) {
    const char* description;
    int code = glfwGetError(&description);
    BAPHY_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    BAPHY_LOG_DEBUG("Created GLFW window");

  if (is_flag_set(params.flags, WindowFlags::borderless)) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(glfw_handle_, base_x, base_y);
  }
#elif defined(BAPHY_PLATFORM_LINUX)
  /* We are making the assumption that the user is running a version of X11
     * that treats *all* fullscreen windows as borderless fullscreen windows.
     * This seems to generally be true for a good majority of systems. This may
     * also just act exactly like a normal fullscreen, there's not really any
     * way to tell ahead of time.
     */

  GLFWmonitor *monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (set(params.flags, WindowFlags::borderless)) {
//    borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }

  glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);
  if (!glfw_handle_) {
    const char *description;
    int code = glfwGetError(&description);
    BAPHY_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    BAPHY_LOG_DEBUG("Created GLFW window");
#endif
}

void Window::open_windowed_(const InitializeParams& params) {
  GLFWmonitor* monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, is_flag_set(params.flags, WindowFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, is_flag_set(params.flags, WindowFlags::undecorated) ? GLFW_FALSE : GLFW_TRUE);

  glfw_handle_ = glfwCreateWindow(params.size.x, params.size.y, params.title.c_str(), nullptr, nullptr);
  if (!glfw_handle_) {
    const char* description;
    int code = glfwGetError(&description);
    BAPHY_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    BAPHY_LOG_DEBUG("Created GLFW window");

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  if (is_flag_set(params.flags, WindowFlags::centered))
    glfwSetWindowPos(
      glfw_handle_,
      base_x + (mode->width - params.size.x) / 2,
      base_y + (mode->height - params.size.y) / 2
    );
  else
    glfwSetWindowPos(
      glfw_handle_,
      base_x + params.pos.x,
      base_y + params.pos.y
    );
}

void Window::r_end_frame_(const E_EndFrame& p) {
  glfwSwapBuffers(glfw_handle_);
}

void Window::r_update_(const E_Update& p) {
  Hermes::poll<E_GlfwWindowClose>(module_name);
}

void Window::r_glfw_window_close_(const E_GlfwWindowClose& p) {
  Hermes::send_nowait<E_ShutdownEngine>();
}
} // namespace baphy
