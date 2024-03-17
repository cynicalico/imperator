#include "imperator/module/window.h"

#include "imperator/core/glfw_callbacks.h"
#include "imperator/util/platform.h"

namespace imp {
Window::Window(std::shared_ptr<Engine> engine, WindowOpenParams open_params)
  : Module(std::move(engine), ModuleInfo<Window>::name) {
  imp_engine->sub_event<E_Update>(imp_module_name, [&](const auto& p) { r_update_(p); });

  imp_engine->sub_event<E_GlfwWindowClose>(imp_module_name, [&](const auto& p) { r_glfw_window_close_(p); });
  imp_engine->sub_event<E_GlfwWindowSize>(imp_module_name, [&](const auto& p) { r_glfw_window_size_(p); });
  imp_engine->sub_event<E_GlfwFramebufferSize>(imp_module_name, [&](const auto& p) { r_glfw_framebuffer_size_(p); });
  imp_engine->sub_event<E_GlfwWindowContentScale>(imp_module_name, [&](const auto& p) {
    r_glfw_window_content_scale_(p);
  });
  imp_engine->sub_event<E_GlfwWindowPos>(imp_module_name, [&](const auto& p) { r_glfw_window_pos_(p); });
  imp_engine->sub_event<E_GlfwWindowIconify>(imp_module_name, [&](const auto& p) { r_glfw_window_iconify_(p); });
  imp_engine->sub_event<E_GlfwWindowMaximize>(imp_module_name, [&](const auto& p) { r_glfw_window_maximize_(p); });
  imp_engine->sub_event<E_GlfwWindowFocus>(imp_module_name, [&](const auto& p) { r_glfw_window_focus_(p); });
  imp_engine->sub_event<E_GlfwWindowRefresh>(imp_module_name, [&](const auto& p) { r_glfw_window_refresh_(p); });
  imp_engine->sub_event<E_GlfwMonitor>(imp_module_name, [&](const auto& p) { r_glfw_monitor_(p); });

  open_(open_params);
}

Window::~Window() {
  clear_glfw_user_pointer(handle());
}

bool Window::should_close() const {
  return glfwWindowShouldClose(handle()) == GLFW_TRUE;
}

void Window::set_should_close(bool v) {
  glfwSetWindowShouldClose(handle(), v ? GLFW_TRUE : GLFW_FALSE);
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

void Window::open_(WindowOpenParams open_params) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, open_params.backend_version.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, open_params.backend_version.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#if !defined(NDEBUG)
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (open_params.mode == WindowMode::fullscreen || open_params.mode == WindowMode::borderless)
    open_fullscreen_(open_params);
  else
    open_windowed_(open_params);

  set_glfw_user_pointer(handle(), imp_engine.get());
  register_glfw_callbacks(handle());

  mode_ = open_params.mode;
  title_ = open_params.title;

  // This isn't sent at the beginning of the program, so we send it first
  // Sent as an immediate event in case we ever do anything special when
  // the window size is changed
  if (open_params.mode == WindowMode::windowed) {
    imp_engine->send_event_nowait<E_GlfwWindowSize>(handle(), open_params.size.x, open_params.size.y);
  }
  else {
    GLFWmonitor* monitor = get_monitor_(open_params.monitor_num);
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    imp_engine->send_event_nowait<E_GlfwWindowSize>(handle(), mode->width, mode->height);
  }

  if (open_params.mode == WindowMode::windowed && !is_flag_set(open_params.flags, WindowFlags::hidden))
    glfwShowWindow(handle());
}

void Window::open_fullscreen_(WindowOpenParams open_params) {
#if defined(IMPERATOR_PLATFORM_WINDOWS)
  GLFWmonitor* monitor = get_monitor_(open_params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  GLFWwindow* h;
  if (open_params.mode == WindowMode::borderless) {
    // borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    // This seems to cause flickering
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    h = glfwCreateWindow(mode->width, mode->height, open_params.title.c_str(), nullptr, nullptr);
  }
  else
    h = glfwCreateWindow(mode->width, mode->height, open_params.title.c_str(), monitor, nullptr);

  if (!h) {
    const char* description;
    int code = glfwGetError(&description);
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glfw_window_ = UniqGLFWwindow(h);
  IMPERATOR_LOG_DEBUG("Created GLFW window");

  if (open_params.mode == WindowMode::borderless) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(handle(), base_x, base_y);
  }
#elif defined(IMPERATOR_PLATFORM_LINUX)
  /* We are making the assumption that the user is running a version of X11
   * that treats *all* fullscreen windows as borderless fullscreen windows.
   * This seems to generally be true for a good majority of systems. This may
   * also just act exactly like a normal fullscreen, there's not really any
   * way to tell ahead of time.
   */

  GLFWmonitor* monitor = get_monitor_(open_params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (open_params.mode == WindowMode::borderless) {
    //    borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }

  auto h = glfwCreateWindow(mode->width, mode->height, open_params.title.c_str(), monitor, nullptr);
  if (!h) {
    const char* description;
    int code = glfwGetError(&description);
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glfw_window_ = UniqGLFWwindow(h);
  IMPERATOR_LOG_DEBUG("Created GLFW window");
#endif
}

void Window::open_windowed_(WindowOpenParams open_params) {
  GLFWmonitor* monitor = get_monitor_(open_params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, is_flag_set(open_params.flags, WindowFlags::resizable)
                                   ? GLFW_TRUE
                                   : GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, is_flag_set(open_params.flags, WindowFlags::undecorated)
                                   ? GLFW_FALSE
                                   : GLFW_TRUE);

  auto h = glfwCreateWindow(open_params.size.x, open_params.size.y,
                            open_params.title.c_str(), nullptr, nullptr);
  if (!h) {
    const char* description;
    int code = glfwGetError(&description);
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glfw_window_ = UniqGLFWwindow(h);
  IMPERATOR_LOG_DEBUG("Created GLFW window");

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  if (is_flag_set(open_params.flags, WindowFlags::centered))
    glfwSetWindowPos(
      handle(),
      base_x + (mode->width - open_params.size.x) / 2,
      base_y + (mode->height - open_params.size.y) / 2
    );
  else
    glfwSetWindowPos(
      handle(),
      base_x + open_params.pos.x,
      base_y + open_params.pos.y
    );
}

void Window::r_update_(const E_Update& p) {
  imp_engine->poll_event<E_GlfwWindowClose>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowSize>(imp_module_name);
  imp_engine->poll_event<E_GlfwFramebufferSize>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowContentScale>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowPos>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowIconify>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowMaximize>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowFocus>(imp_module_name);
  imp_engine->poll_event<E_GlfwWindowRefresh>(imp_module_name);
  imp_engine->poll_event<E_GlfwMonitor>(imp_module_name);
}

void Window::r_glfw_window_close_(const E_GlfwWindowClose& p) {}

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
