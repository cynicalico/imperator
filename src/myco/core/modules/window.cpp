#include "myco/util/log.hpp"

#include "myco/core/modules/input_mgr.hpp"
#include "myco/core/modules/window.hpp"
#include "myco/core/engine.hpp"
#include "myco/core/glfw_callbacks.hpp"

#include "myco/util/io.hpp"
#include "myco/util/platform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <set>

#if defined(MYCO_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

namespace myco {

std::once_flag Window::initialized_glfw_;

void Window::open(const WindowOpenParams &params) {
  open_params_ = params;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, params.backend_version.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, params.backend_version.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (set(params.flags, WindowFlags::fullscreen) || set(params.flags, WindowFlags::borderless))
    open_fullscreen_(params);
  else
    open_windowed_(params);

  register_glfw_callbacks(glfw_handle);

  glfwMakeContextCurrent(glfw_handle);

  vsync = set(params.flags, myco::WindowFlags::vsync);
  glfwSwapInterval(vsync ? 1 : 0);

#if defined(MYCO_PLATFORM_WINDOWS)
  win32_hwnd_ = glfwGetWin32Window(glfw_handle);
  win32_saved_WndProc_ = (WNDPROC)GetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC);
  win32_force_light_mode_ = params.win32_force_light_mode;
  win32_force_dark_mode_ = params.win32_force_dark_mode;

  // Set up our Win32 pointers for callbacks
  SetWindowLongPtr(win32_hwnd_, GWLP_USERDATA, (LONG_PTR)this);
  SetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC, (LONG_PTR)WndProc_);

  set_win32_titlebar_color_(win32_hwnd_);
#endif

  set_icon_dir(DATA / "icon");

  if (!set(params.flags, WindowFlags::fullscreen) &&
      !set(params.flags, WindowFlags::borderless) &&
      !set(params.flags, WindowFlags::hidden))
    glfwShowWindow(glfw_handle);
}

WindowOpenParams Window::get_open_params() const {
  return open_params_;
}

void Window::set_icon_dir(const std::filesystem::path &dir) {
  // This is the list of formats that stb_image can support
  static std::set<std::string> valid_extensions = {
      ".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"};

  std::vector<std::filesystem::path> icon_paths{};
  for (const auto &p: std::filesystem::directory_iterator(dir))
    if (p.is_regular_file() && valid_extensions.contains(p.path().extension().string()))
      icon_paths.emplace_back(p.path());

  if (icon_paths.empty())
    MYCO_LOG_WARN("No valid icon images found in '{}'", dir.string());

  set_icon(icon_paths);
}

void Window::set_icon(const std::vector<std::filesystem::path> &paths) {
  std::vector<GLFWimage> images{};
  for (const auto &p : paths) {
    images.emplace_back();
    images.back().pixels = stbi_load(p.string().c_str(), &images.back().width, &images.back().height, 0, 4);
  }
  glfwSetWindowIcon(glfw_handle, images.size(), &images[0]);

  for (auto &i : images)
    stbi_image_free(i.pixels);
}

void Window::set_icon(const std::filesystem::path &path) {
  std::vector<std::filesystem::path> paths = {path};
  set_icon(paths);
}

void Window::set_x(int xpos) {
  x = xpos;
  glfwSetWindowPos(glfw_handle, xpos, y);
}

void Window::set_y(int ypos) {
  y = ypos;
  glfwSetWindowPos(glfw_handle, x, ypos);
}

void Window::set_w(int width) {
  w = width;
  glfwSetWindowSize(glfw_handle, width, h);
}

void Window::set_h(int height) {
  h = height;
  glfwSetWindowSize(glfw_handle, w, height);
}

void Window::set_decorated(bool is_decorated) {
  decorated = is_decorated;
  glfwSetWindowAttrib(glfw_handle, GLFW_DECORATED, is_decorated ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_resizable(bool is_resizable) {
  resizable = is_resizable;
  glfwSetWindowAttrib(glfw_handle, GLFW_RESIZABLE, is_resizable ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_vsync(bool is_enabled) {
  vsync = is_enabled;
  glfwSwapInterval(vsync ? 1 : 0);
}

bool Window::should_close() const {
  return glfwWindowShouldClose(glfw_handle);
}

void Window::set_should_close(bool should) {
  glfwSetWindowShouldClose(glfw_handle, should ? GLFW_TRUE : GLFW_FALSE);
}

void Window::swap() const {
  glfwSwapBuffers(glfw_handle);
}

void Window::initialize_(const Initialize &e) {
  Module::initialize_(e);

  Scheduler::sub<Update>(name, {ModuleInfo<InputMgr>::name}, [&](const auto &e) { update_(e.dt); });

  Scheduler::sub<StartFrame>(name, [&](const auto &e) { start_frame_(e); });
  Scheduler::sub<EndFrame>(name, {ModuleInfo<Application>::name}, [&](const auto &e) { end_frame_(e); });

  Scheduler::sub<GlfwWindowClose>(name, [&](const auto &e) { close_callback_(e); });

  std::call_once(initialized_glfw_, []() {
    if (glfwInit() == GLFW_FALSE)
      std::exit(EXIT_FAILURE);

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    MYCO_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, revision);
  });
}

void Window::update_(double dt) {
  glfwPollEvents();
}

void Window::start_frame_(const StartFrame &e) {}

void Window::end_frame_(const EndFrame &e) {
  swap();

  // Check if the user closed the window without the close button
  if (should_close())
    Scheduler::send_nowait<GlfwWindowClose>(glfw_handle);
}

void Window::close_callback_(const GlfwWindowClose &e) {
  Scheduler::send_nowait<Shutdown>();
}

GLFWmonitor *Window::get_monitor_(int monitor_num) {
  int monitor_count = 0;
  auto monitors = glfwGetMonitors(&monitor_count);

  if (monitor_num >= monitor_count) {
    MYCO_LOG_WARN("Monitor {} out of range (only {} monitors available); defaulting to primary", monitor_num,
                  monitor_count);
    return monitors[0];
  }
  return monitors[monitor_num];
}

void Window::open_fullscreen_(const WindowOpenParams &params) {
#if defined(MYCO_PLATFORM_WINDOWS)
  GLFWmonitor *monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (set(params.flags, WindowFlags::borderless)) {
    borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    glfw_handle = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), nullptr, nullptr);
  } else
    glfw_handle = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);

  if (!glfw_handle) {
    const char *description;
    int code = glfwGetError(&description);
    MYCO_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    MYCO_LOG_DEBUG("Created GLFW window");

  w = mode->width;
  h = mode->height;

  if (set(params.flags, WindowFlags::borderless)) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(glfw_handle, base_x, base_y);
  }
#elif defined(MYCO_PLATFORM_LINUX)
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
    borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }

  glfw_handle = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);
  if (!glfw_handle) {
    const char *description;
    int code = glfwGetError(&description);
    MYCO_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    MYCO_LOG_DEBUG("Created GLFW window");

  w = mode->width;
  h = mode->height;
#endif
}

void Window::open_windowed_(const WindowOpenParams &params) {
  GLFWmonitor *monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, set(params.flags, WindowFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);
  if (set(params.flags, WindowFlags::undecorated)) {
    decorated = false;
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  }

  glfw_handle = glfwCreateWindow(params.size.x, params.size.y, params.title.c_str(), nullptr, nullptr);
  if (!glfw_handle) {
    const char *description;
    int code = glfwGetError(&description);
    MYCO_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    MYCO_LOG_DEBUG("Created GLFW window");

  w = params.size.x;
  h = params.size.y;

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  if (set(params.flags, WindowFlags::centered))
    glfwSetWindowPos(
        glfw_handle,
        base_x + (mode->width - params.size.x) / 2,
        base_y + (mode->height - params.size.y) / 2
    );
  else
    glfwSetWindowPos(
        glfw_handle,
        base_x + params.pos.x,
        base_y + params.pos.y
    );
}

/*******************************************************************************
 * This code is specifically for setting the titlebar to the dark mode in
 * Windows. This seems to work on both Windows 10 and 11, thought the documentation
 * on this page is specific to Windows 11.
 *
 * https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/apply-windows-themes
 *
 * I would assume there are some older versions of Windows 10 that this does not work on,
 * if anyone ever uses this and has a problem then it can be adjusted
 */
#if defined(MYCO_PLATFORM_WINDOWS)
void Window::set_win32_titlebar_color_(HWND hwnd) {
  auto window = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  DWORD should_use_light_theme{};
  DWORD should_use_light_theme_size = sizeof(should_use_light_theme);
  LONG code = RegGetValue(
      HKEY_CURRENT_USER,
      R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
      "AppsUseLightTheme",
      RRF_RT_REG_DWORD,
      nullptr,
      &should_use_light_theme,
      &should_use_light_theme_size
  );

  if (code != ERROR_SUCCESS) {
    MYCO_LOG_WARN("Failed to read Windows app theme from registry, error code {}", code);
    return;
  }

  if ((should_use_light_theme || window->win32_force_light_mode_) && !window->win32_force_dark_mode_) {
    const BOOL use_light_mode = 0;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &use_light_mode, sizeof(use_light_mode));
  } else if ((!should_use_light_theme || window->win32_force_dark_mode_) && !window->win32_force_light_mode_) {
    const BOOL use_dark_mode = 1;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &use_dark_mode, sizeof(use_dark_mode));
  }
  UpdateWindow(window->win32_hwnd_);
}

LRESULT CALLBACK Window::WndProc_(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  auto window = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  // TODO: Make this check more specific, this goes off at times where the
  //  titlebar didn't change at all, but other style elements *did* change
  if (message == WM_SETTINGCHANGE && hwnd == window->win32_hwnd_)
    set_win32_titlebar_color_(hwnd);


  return CallWindowProc(window->win32_saved_WndProc_, hwnd, message, wParam, lParam);
}
#endif

} // namespace myco
