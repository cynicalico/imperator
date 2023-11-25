#include "imperator/core/module/window.hpp"

#include "imperator/core/module/application.hpp"
#include "imperator/core/module/glfw_callbacks.hpp"
#include "imperator/util/log.hpp"
#include "glm/ext/matrix_clip_space.hpp"

namespace imp {
std::once_flag Window::initialize_glfw_;

glm::mat4 Window::projection_matrix() const {
  return glm::ortho(0.0f, static_cast<float>(size_.x), static_cast<float>(size_.y), 0.0f);
}

void Window::r_initialize_(const E_Initialize& p) {
  Hermes::sub<E_EndFrame>(module_name, {EPI<Application>::name}, [&](const auto& p) { r_end_frame_(p); });
  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });

  Hermes::sub<E_GlfwWindowClose>(module_name, [&](const auto& p) { r_glfw_window_close_(p); });
  Hermes::sub<E_GlfwWindowSize>(module_name, [&](const auto& p) { r_glfw_window_size_(p); });
  Hermes::sub<E_GlfwFramebufferSize>(module_name, [&](const auto& p) { r_glfw_framebuffer_size_(p); });
  Hermes::sub<E_GlfwWindowContentScale>(module_name, [&](const auto& p) { r_glfw_window_content_scale_(p); });
  Hermes::sub<E_GlfwWindowPos>(module_name, [&](const auto& p) { r_glfw_window_pos_(p); });
  Hermes::sub<E_GlfwWindowIconify>(module_name, [&](const auto& p) { r_glfw_window_iconify_(p); });
  Hermes::sub<E_GlfwWindowMaximize>(module_name, [&](const auto& p) { r_glfw_window_maximize_(p); });
  Hermes::sub<E_GlfwWindowFocus>(module_name, [&](const auto& p) { r_glfw_window_focus_(p); });
  Hermes::sub<E_GlfwWindowRefresh>(module_name, [&](const auto& p) { r_glfw_window_refresh_(p); });
  Hermes::sub<E_GlfwMonitor>(module_name, [&](const auto& p) { r_glfw_monitor_(p); });

  std::call_once(initialize_glfw_, [&]() {
    register_glfw_error_callback();

    if (glfwInit() == GLFW_FALSE) {
      IMPERATOR_LOG_CRITICAL("Failed to initialize GLFW");
      std::exit(EXIT_FAILURE);
    }

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    IMPERATOR_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, revision);
  });

  open_(p.params);

  Module::r_initialize_(p);
}

void Window::r_shutdown_(const E_Shutdown& p) {
  glfwTerminate();
  IMPERATOR_LOG_DEBUG("Terminated GLFW");

  Module::r_shutdown_(p);
}

GLFWmonitor* Window::get_monitor_(int monitor_num) {
  int monitor_count = 0;
  const auto monitors = glfwGetMonitors(&monitor_count);

  if (monitor_num >= monitor_count) {
    IMPERATOR_LOG_WARN(
      "Monitor {} out of range ({} available); defaulting to primary",
      monitor_num,
      monitor_count
    );
    return monitors[0];
  }
  return monitors[monitor_num];
}

void Window::open_(const InitializeParams& params) {
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

  // This isn't sent at the beginning of the program, so we send it first
  // Sent as an immediate event in case we ever do anything special when
  // the window size is changed
  Hermes::send_nowait<E_GlfwWindowSize>(glfw_handle_, params.size.x, params.size.y);

  register_glfw_callbacks(glfw_handle_);

  if (!is_flag_set(params.flags, WindowFlags::fullscreen) &&
    !is_flag_set(params.flags, WindowFlags::borderless) &&
    !is_flag_set(params.flags, WindowFlags::hidden))
    glfwShowWindow(glfw_handle_);
}

void Window::open_fullscreen_(const InitializeParams& params) {
#if defined(IMPERATOR_PLATFORM_WINDOWS)
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
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    IMPERATOR_LOG_DEBUG("Created GLFW window");

  if (is_flag_set(params.flags, WindowFlags::borderless)) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(glfw_handle_, base_x, base_y);
  }
#elif defined(IMPERATOR_PLATFORM_LINUX)
  /* We are making the assumption that the user is running a version of X11
     * that treats *all* fullscreen windows as borderless fullscreen windows.
     * This seems to generally be true for a good majority of systems. This may
     * also just act exactly like a normal fullscreen, there's not really any
     * way to tell ahead of time.
     */

  GLFWmonitor* monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (is_flag_set(params.flags, WindowFlags::borderless)) {
    //    borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }

  glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);
  if (!glfw_handle_) {
    const char* description;
    int code = glfwGetError(&description);
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    IMPERATOR_LOG_DEBUG("Created GLFW window");
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
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  IMPERATOR_LOG_DEBUG("Created GLFW window");

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
  Hermes::poll<E_GlfwWindowSize>(module_name);
  Hermes::poll<E_GlfwFramebufferSize>(module_name);
  Hermes::poll<E_GlfwWindowContentScale>(module_name);
  Hermes::poll<E_GlfwWindowPos>(module_name);
  Hermes::poll<E_GlfwWindowIconify>(module_name);
  Hermes::poll<E_GlfwWindowMaximize>(module_name);
  Hermes::poll<E_GlfwWindowFocus>(module_name);
  Hermes::poll<E_GlfwWindowRefresh>(module_name);
  Hermes::poll<E_GlfwMonitor>(module_name);
}

void Window::r_glfw_window_close_(const E_GlfwWindowClose& p) {
  Hermes::send_nowait<E_ShutdownEngine>();
}

void Window::r_glfw_window_size_(const E_GlfwWindowSize& p) {
  size_.x = p.width;
  size_.y = p.height;
}

void Window::r_glfw_framebuffer_size_(const E_GlfwFramebufferSize& p) {}
void Window::r_glfw_window_content_scale_(const E_GlfwWindowContentScale& p) {}

void Window::r_glfw_window_pos_(const E_GlfwWindowPos& p) {
  pos_.x = p.xpos;
  pos_.y = p.ypos;
}

void Window::r_glfw_window_iconify_(const E_GlfwWindowIconify& p) {}
void Window::r_glfw_window_maximize_(const E_GlfwWindowMaximize& p) {}
void Window::r_glfw_window_focus_(const E_GlfwWindowFocus& p) {}
void Window::r_glfw_window_refresh_(const E_GlfwWindowRefresh& p) {}
void Window::r_glfw_monitor_(const E_GlfwMonitor& p) {}
} // namespace imp
