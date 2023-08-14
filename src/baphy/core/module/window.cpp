#include "baphy/core/module/window.hpp"

#include "baphy/core/module/application.hpp"
#include "baphy/core/glfw_callbacks.hpp"
#include "baphy/util/io.hpp"
#include <set>

#if defined(BAPHY_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <dwmapi.h>

#pragma comment(lib, "ntdll.lib")

extern "C" {
typedef LONG NTSTATUS, *PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)

// Windows 2000 and newer
NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOEXW lpVersionInformation);
}

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#endif

namespace baphy {

std::once_flag Window::initialize_glfw_;

GLFWwindow *Window::handle() {
  return glfw_handle_;
}

void Window::open_(const baphy::InitializeParams &params) {
  open_params_ = params;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, params.backend_version.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, params.backend_version.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (set(params.flags, WindowFlags::fullscreen) || set(params.flags, WindowFlags::borderless))
    open_fullscreen_(params);
  else
    open_windowed_(params);

  register_glfw_callbacks(glfw_handle_);

#if defined(BAPHY_PLATFORM_WINDOWS)
  win32_hwnd_ = glfwGetWin32Window(glfw_handle_);
  win32_saved_WndProc_ = (WNDPROC)GetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC);
  win32_force_light_mode_ = params.win32_force_light_mode;
  win32_force_dark_mode_ = params.win32_force_dark_mode;

  // Set up our Win32 pointers for callbacks
  SetWindowLongPtr(win32_hwnd_, GWLP_USERDATA, (LONG_PTR)this);
  SetWindowLongPtr(win32_hwnd_, GWLP_WNDPROC, (LONG_PTR)WndProc_);

  set_win32_titlebar_color_(win32_hwnd_);

  RTL_OSVERSIONINFOEXW win32_os_ver;
  win32_os_ver.dwOSVersionInfoSize = sizeof(win32_os_ver);
  NTSTATUS status = RtlGetVersion(&win32_os_ver);
  assert(status == STATUS_SUCCESS);

  if (win32_os_ver.dwBuildNumber > 22000 && params.win32_force_square_corners) {
    DWORD p = 1;
    DwmSetWindowAttribute(win32_hwnd_, DWMWA_WINDOW_CORNER_PREFERENCE, &p, sizeof(p));
  }
#endif

  set_icon_dir(DATA_FOLDER / "icon");

  if (!set(params.flags, WindowFlags::fullscreen) &&
      !set(params.flags, WindowFlags::borderless) &&
      !set(params.flags, WindowFlags::hidden))
    glfwShowWindow(glfw_handle_);
}

InitializeParams Window::open_params() const {
  return open_params_;
}

void Window::make_context_current() {
  glfwMakeContextCurrent(glfw_handle_);
}

int Window::x() const {
  // TODO: handle different monitors
  int x;
  glfwGetWindowPos(glfw_handle_, &x, nullptr);

  return x;
}

int Window::y() const {
  // TODO: handle different monitors
  int y;
  glfwGetWindowPos(glfw_handle_, nullptr, &y);

  return y;
}

glm::ivec2 Window::pos() const {
  // TODO: handle different monitors
  int x, y;
  glfwGetWindowPos(glfw_handle_, &x, &y);

  return {x, y};
}

int Window::w() const {
  int w;
  glfwGetWindowSize(glfw_handle_, &w, nullptr);

  return w;
}

int Window::h() const {
  int h;
  glfwGetWindowSize(glfw_handle_, nullptr, &h);

  return h;
}

glm::ivec2 Window::size() const {
  int w, h;
  glfwGetWindowSize(glfw_handle_, &w, &h);

  return {w, h};
}

bool Window::is_minimized() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_ICONIFIED) == GLFW_TRUE;
}

bool Window::is_maximized() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_MAXIMIZED) == GLFW_TRUE;
}

bool Window::is_visible() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_VISIBLE) == GLFW_TRUE;
}

bool Window::is_resizable() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_RESIZABLE) == GLFW_TRUE;
}

bool Window::is_decorated() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_DECORATED) == GLFW_TRUE;
}

bool Window::is_auto_iconify() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_AUTO_ICONIFY) == GLFW_TRUE;
}

bool Window::is_floating() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_FLOATING) == GLFW_TRUE;
}

bool Window::is_focus_on_show() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_FOCUS_ON_SHOW) == GLFW_TRUE;
}

bool Window::should_close() const {
  return glfwWindowShouldClose(glfw_handle_) == GLFW_TRUE;
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
    BAPHY_LOG_WARN("No valid icon images found in '{}'", dir.string());

  set_icon(icon_paths);
}

void Window::set_icon(const std::vector<std::filesystem::path> &paths) {
  std::vector<ImageData> images{};
  for (const auto &p: paths)
    images.emplace_back(p, 4);

  std::vector<GLFWimage> glfw_images{};
  for (auto &i: images)
    glfw_images.emplace_back(i.glfw_image());
  glfwSetWindowIcon(glfw_handle_, glfw_images.size(), &glfw_images[0]);
}

void Window::set_icon(const std::filesystem::path &path) {
  std::vector<std::filesystem::path> paths = {path};
  set_icon(paths);
}

void Window::set_x(int x) {
  // TODO: handle different monitors
  glfwSetWindowPos(glfw_handle_, x, y());
}

void Window::set_y(int y) {
  // TODO: handle different monitors
  glfwSetWindowPos(glfw_handle_, x(), y);
}

void Window::set_pos(int x, int y) {
  // TODO: handle different monitors
  glfwSetWindowPos(glfw_handle_, x, y);
}

void Window::center(int monitor_num) {
  // TODO: center on whatever monitor was opened on by default

  GLFWmonitor *monitor = get_monitor_(monitor_num);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  glfwSetWindowPos(
      glfw_handle_,
      base_x + (mode->width - w()) / 2,
      base_y + (mode->height - h()) / 2
  );
}

void Window::set_w(int w) {
  glfwSetWindowSize(glfw_handle_, w, h());
}

void Window::set_h(int h) {
  glfwSetWindowSize(glfw_handle_, w(), h);
}

void Window::set_size(int w, int h) {
  glfwSetWindowSize(glfw_handle_, w, h);
}

void Window::minimize() {
  glfwIconifyWindow(glfw_handle_);
}

void Window::maximize() {
  glfwMaximizeWindow(glfw_handle_);
}

void Window::restore() {
  glfwRestoreWindow(glfw_handle_);
}

void Window::show() {
  glfwShowWindow(glfw_handle_);
}

void Window::hide() {
  glfwHideWindow(glfw_handle_);
}

void Window::set_resizable(bool v) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_RESIZABLE, v ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_decorated(bool v) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_DECORATED, v ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_auto_iconify(bool v) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_AUTO_ICONIFY, v ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_floating(bool v) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_FLOATING, v ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_focus_on_show(bool v) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_FOCUS_ON_SHOW, v ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_should_close(bool v) {
  glfwSetWindowShouldClose(glfw_handle_, v ? GLFW_TRUE : GLFW_FALSE);
}

void Window::swap() const {
  glfwSwapBuffers(glfw_handle_);
}

GLFWmonitor *Window::get_monitor_(int monitor_num) {
  int monitor_count = 0;
  auto monitors = glfwGetMonitors(&monitor_count);

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

void Window::open_fullscreen_(const InitializeParams &params) {
#if defined(BAPHY_PLATFORM_WINDOWS)
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
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), nullptr, nullptr);
  } else
    glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);

  if (!glfw_handle_) {
    const char *description;
    int code = glfwGetError(&description);
    BAPHY_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    BAPHY_LOG_DEBUG("Created GLFW window");

  if (set(params.flags, WindowFlags::borderless)) {
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

void Window::open_windowed_(const InitializeParams &params) {
  GLFWmonitor *monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, set(params.flags, WindowFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);

  glfw_handle_ = glfwCreateWindow(params.size.x, params.size.y, params.title.c_str(), nullptr, nullptr);
  if (!glfw_handle_) {
    const char *description;
    int code = glfwGetError(&description);
    BAPHY_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    BAPHY_LOG_DEBUG("Created GLFW window");

  int base_x, base_y;
  glfwGetMonitorPos(monitor, &base_x, &base_y);
  if (set(params.flags, WindowFlags::centered))
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

void Window::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });
  EventBus::sub<EStartFrame>(module_name, [&](const auto &e) { e_start_frame_(e); });
  EventBus::sub<EEndFrame>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_end_frame_(e); });
  EventBus::sub<EGlfwWindowClose>(module_name, [&](const auto &e) { e_glfw_window_close_(e); });

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

  open_(e.params);

  Module::e_initialize_(e);
}

void Window::e_shutdown_(const baphy::EShutdown &e) {
  glfwTerminate();
  BAPHY_LOG_DEBUG("Terminated GLFW");

  Module::e_shutdown_(e);
}

void Window::e_update_(const EUpdate &e) {
  EventBus::poll<EGlfwWindowClose>(module_name);

  if (!no_more_polling_)
    glfwPollEvents();
}

void Window::e_start_frame_(const EStartFrame &e) {}

void Window::e_end_frame_(const EEndFrame &e) {
  swap();

  if (should_close())
    EventBus::send_nowait<EGlfwWindowClose>(glfw_handle_);
}

void Window::e_glfw_window_close_(const EGlfwWindowClose &e) {
  no_more_polling_ = true;

  EventBus::send_nowait<EShutdown>();
}

/*******************************************************************************
 * This code is specifically for setting the titlebar to the dark mode in
 * Windows. This seems to work on both Windows 10 and 11, though the documentation
 * on this page is specific to Windows 11.
 *
 * https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/apply-windows-themes
 *
 * I would assume there are some older versions of Windows 10 that this does not work on,
 * if anyone ever uses this and has a problem then it can be adjusted
 */
#if defined(BAPHY_PLATFORM_WINDOWS)
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
    BAPHY_LOG_WARN("Failed to read Windows app theme from registry, error code {}", code);
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

} // namespace baphy
