#include "imp/core/module/window.hpp"

#include "imp/core/module/application.hpp"
#include "imp/core/module/glfw_callbacks.hpp"
#include "imp/util/io.hpp"
#include "imp/util/log.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <set>

namespace imp {
std::once_flag Window::initialize_glfw_;

bool Window::should_close() const {
  return glfwWindowShouldClose(glfw_handle_);
}

bool Window::resizable() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_RESIZABLE);
}

bool Window::decorated() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_DECORATED);
}

bool Window::auto_iconify() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_AUTO_ICONIFY);
}

bool Window::floating() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_FLOATING);
}

bool Window::focus_on_show() const {
  return glfwGetWindowAttrib(glfw_handle_, GLFW_FOCUS_ON_SHOW);
}

glm::mat4 Window::projection_matrix() const {
  return glm::ortho(0.0f, static_cast<float>(size_.x), static_cast<float>(size_.y), 0.0f);
}

void Window::set_should_close(bool should_close) {
  glfwSetWindowShouldClose(glfw_handle_, should_close ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_resizable(bool resizable) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_decorated(bool decorated) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_auto_iconify(bool auto_iconify) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_AUTO_ICONIFY, auto_iconify ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_floating(bool floating) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_FLOATING, floating ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_focus_on_show(bool focus_on_show) {
  glfwSetWindowAttrib(glfw_handle_, GLFW_FOCUS_ON_SHOW, focus_on_show ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_pos(int x, int y) {
  glfwSetWindowPos(glfw_handle_, x, y);
}

void Window::set_x(int x) {
  set_pos(x, pos_.y);
}

void Window::set_y(int y) {
  set_pos(pos_.x, y);
}

void Window::set_size(int w, int h) {
  glfwSetWindowSize(glfw_handle_, w, h);
}

void Window::set_w(int w) {
  set_size(w, size_.y);
}

void Window::set_h(int h) {
  set_size(size_.x, h);
}

void Window::set_title(const std::string& title) {
  glfwSetWindowTitle(glfw_handle_, title.c_str());
}

void Window::set_icon(const std::vector<std::filesystem::path>& paths) {
  std::vector<ImageData> images{};
  for (const auto& path: paths)
    images.emplace_back(path, 4);

  std::vector<GLFWimage> glfw_images{};
  for (auto& i: images)
    glfw_images.emplace_back(i.glfw_image());

  glfwSetWindowIcon(glfw_handle_, glfw_images.size(), &glfw_images[0]);
}

void Window::set_icon(const std::filesystem::path& path) {
  const std::vector paths = {path};
  set_icon(paths);
}

void Window::set_icon_dir(const std::filesystem::path& dir) {
  // This is the list of formats that stb_image can support
  static std::set<std::string> valid_extensions = {
    ".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"
  };

  std::vector<std::filesystem::path> icon_paths{};
  for (const auto& p: std::filesystem::directory_iterator(dir)) {
    if (p.is_regular_file() && valid_extensions.contains(p.path().extension().string())) {
      icon_paths.emplace_back(p.path());
    }
  }

  if (icon_paths.empty()) {
    IMP_LOG_WARN("No valid icon images found in '{}'", dir.string());
  }

  set_icon(icon_paths);
}

void Window::set_monitor(WindowMode mode, int monitor_num, int x, int y, int w, int h) {
  if (mode == WindowMode::windowed) {
    glfwSetWindowMonitor(glfw_handle_, nullptr, x, y, w, h, 0);
  } else {
    GLFWmonitor* monitor = get_monitor_(monitor_num);
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

#if defined(IMP_PLATFORM_WINDOWS)
    if (mode == WindowMode::borderless) {
      set_auto_iconify(false);
      set_floating(true);
      int base_x, base_y;
      glfwGetMonitorPos(monitor, &base_x, &base_y);
      glfwSetWindowMonitor(glfw_handle_, nullptr, base_x, base_y, vidmode->width, vidmode->height, 0);
    } else {
      set_auto_iconify(true);
      set_floating(false);
      glfwSetWindowMonitor(glfw_handle_, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
    }
#elif defined(IMP_PLATFORM_LINUX)
    set_auto_iconify(mode != WindowMode::borderless);
    glfwSetWindowMonitor(glfw_handle_, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
#endif
  }

  mode_ = mode;
}

void Window::r_initialize_(const E_Initialize& p) {
  debug_overlay = module_mgr->get<DebugOverlay>();

  IMP_HERMES_SUB(E_EndFrame, module_name, r_end_frame_, Application);
  IMP_HERMES_SUB(E_Update, module_name, r_update_);

  IMP_HERMES_SUB(E_GlfwWindowClose, module_name, r_glfw_window_close_);
  IMP_HERMES_SUB(E_GlfwWindowSize, module_name, r_glfw_window_size_);
  IMP_HERMES_SUB(E_GlfwFramebufferSize, module_name, r_glfw_framebuffer_size_);
  IMP_HERMES_SUB(E_GlfwWindowContentScale, module_name, r_glfw_window_content_scale_);
  IMP_HERMES_SUB(E_GlfwWindowPos, module_name, r_glfw_window_pos_);
  IMP_HERMES_SUB(E_GlfwWindowIconify, module_name, r_glfw_window_iconify_);
  IMP_HERMES_SUB(E_GlfwWindowMaximize, module_name, r_glfw_window_maximize_);
  IMP_HERMES_SUB(E_GlfwWindowFocus, module_name, r_glfw_window_focus_);
  IMP_HERMES_SUB(E_GlfwWindowRefresh, module_name, r_glfw_window_refresh_);
  IMP_HERMES_SUB(E_GlfwMonitor, module_name, r_glfw_monitor_);

  std::call_once(initialize_glfw_, [&]() {
    register_glfw_error_callback();

    if (glfwInit() == GLFW_FALSE) {
      IMP_LOG_CRITICAL("Failed to initialize GLFW");
      std::exit(EXIT_FAILURE);
    }

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    IMP_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, revision);
  });

  monitors_ = glfwGetMonitors(&monitor_count_);
  open_(p.params);
  overlay_.current_mode = mode_ == WindowMode::windowed ? 0 : mode_ == WindowMode::fullscreen ? 1 : 2;
  overlay_.current_monitor = detect_monitor_num();

  debug_overlay->add_tab(module_name, [&] {
    ImGui::SeparatorText("Mode/Pos/Size");

    // ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::BeginCombo("Mode", overlay_.modes[overlay_.current_mode].second.c_str())) {
      for (std::size_t i = 0; i < overlay_.modes.size(); ++i) {
        const bool is_selected = overlay_.current_mode == i;
        if (ImGui::Selectable(overlay_.modes[i].second.c_str(), is_selected)) {
          overlay_.current_mode = i;
        }
        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    // ImGui::PopItemWidth();

    const auto mname = [](const char* name, int i) {
      return fmt::format("{}: {}", i, name);
    };
    // ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::BeginCombo("Monitor",
                          mname(glfwGetMonitorName(monitors_[overlay_.current_monitor]),
                                overlay_.current_monitor).c_str())) {
      for (std::size_t i = 0; i < monitor_count_; ++i) {
        const bool is_selected = overlay_.current_monitor == i;
        if (ImGui::Selectable(mname(glfwGetMonitorName(monitors_[i]), i).c_str(), is_selected)) {
          overlay_.current_monitor = i;
        }
        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    // ImGui::PopItemWidth();

    if (overlay_.modes[overlay_.current_mode].first != WindowMode::windowed) { ImGui::BeginDisabled(); }
    ImGui::InputInt2("pos", overlay_.pos);
    ImGui::InputInt2("size", overlay_.size);
    if (overlay_.modes[overlay_.current_mode].first != WindowMode::windowed) { ImGui::EndDisabled(); }

    const int n = 3;
    const auto item_spacing = ImGui::GetStyle().ItemSpacing;
    const auto avail_space = ImGui::GetContentRegionAvail();
    const float mrw = (avail_space.x - item_spacing.x * (n - 1)) / static_cast<float>(n);

    if (ImGui::Button("Set", {mrw, 0})) {
      set_monitor(
        overlay_.modes[overlay_.current_mode].first,
        overlay_.current_monitor,
        overlay_.pos[0], overlay_.pos[1],
        overlay_.size[0], overlay_.size[1]
      );
    }

    if (mode_ != WindowMode::windowed) { ImGui::BeginDisabled(); }
    ImGui::SameLine();
    if (ImGui::Button("Center", {mrw, 0}) && mode_ == WindowMode::windowed) {
      int base_x, base_y;
      glfwGetMonitorPos(monitors_[overlay_.current_monitor], &base_x, &base_y);
      auto vidmode = glfwGetVideoMode(monitors_[overlay_.current_monitor]);
      set_pos(
        base_x + (vidmode->width - size_.x) / 2,
        base_y + (vidmode->height - size_.y) / 2
      );
    }
    if (mode_ != WindowMode::windowed) { ImGui::EndDisabled(); }

    ImGui::SameLine();
    if (ImGui::Button("Clear", {mrw, 0})) {
      overlay_.current_mode = mode_ == WindowMode::windowed ? 0 : mode_ == WindowMode::fullscreen ? 1 : 2;
      overlay_.current_monitor = detect_monitor_num();
      overlay_.pos[0] = pos_.x;
      overlay_.pos[1] = pos_.y;
      overlay_.size[0] = size_.x;
      overlay_.size[1] = size_.y;
    }

    ImGui::SeparatorText("Attributes");

    if (bool v = resizable(); ImGui::Checkbox("Resizable", &v)) {
      set_resizable(v);
    }

    if (bool v = decorated(); ImGui::Checkbox("Decorated", &v)) {
      set_decorated(v);
    }

    if (bool v = auto_iconify(); ImGui::Checkbox("Auto iconify", &v)) {
      set_auto_iconify(v);
    }

    if (bool v = floating(); ImGui::Checkbox("Floating", &v)) {
      set_floating(v);
    }

    if (bool v = focus_on_show(); ImGui::Checkbox("Focus on show", &v)) {
      set_focus_on_show(v);
    }
  });

  Module::r_initialize_(p);
}

void Window::r_shutdown_(const E_Shutdown& p) {
  glfwTerminate();
  IMP_LOG_DEBUG("Terminated GLFW");

  Module::r_shutdown_(p);
}

int Window::detect_monitor_num() const {
  if (mode_ == WindowMode::windowed) {
    for (std::size_t i = 0; i < monitor_count_; ++i) {
      int mx, my;
      glfwGetMonitorPos(monitors_[i], &mx, &my);
      auto vidmode = glfwGetVideoMode(monitors_[i]);

      if (pos_.x >= mx && pos_.x < mx + vidmode->width &&
          pos_.y >= my && pos_.y < my + vidmode->height) {
        return i;
      }
    }

    // Nothing matched, means the top left corner is offscreen, return default
    return 0;
  }

  const auto m = glfwGetWindowMonitor(glfw_handle_);
  for (std::size_t i = 0; i < monitor_count_; ++i) {
    if (m == monitors_[i]) {
      return i;
    }
  }

  // Nothing matched, return default
  return 0;
}

GLFWmonitor* Window::get_monitor_(int monitor_num) {
  int monitor_count = 0;
  const auto monitors = glfwGetMonitors(&monitor_count);

  if (monitor_num >= monitor_count) {
    IMP_LOG_WARN(
      "Monitor {} out of range ({} available); defaulting to primary",
      monitor_num,
      monitor_count
    );
    return monitors[0];
  }
  return monitors[monitor_num];
}

#if defined(IMP_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <dwmapi.h>

#pragma comment(lib, "ntdll.lib") // For RtlGetVersion
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

void Window::open_(const InitializeParams& params) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, params.backend_version.x);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, params.backend_version.y);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#if !defined(NDEBUG)
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (params.mode == WindowMode::fullscreen || params.mode == WindowMode::borderless)
    open_fullscreen_(params);
  else
    open_windowed_(params);

  mode_ = params.mode;
  title_ = params.title;

  // This isn't sent at the beginning of the program, so we send it first
  // Sent as an immediate event in case we ever do anything special when
  // the window size is changed
  if (params.mode == WindowMode::windowed) {
    Hermes::send_nowait<E_GlfwWindowSize>(glfw_handle_, params.size.x, params.size.y);
  } else {
    GLFWmonitor* monitor = get_monitor_(params.monitor_num);
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    Hermes::send_nowait<E_GlfwWindowSize>(glfw_handle_, mode->width, mode->height);
  }

  register_glfw_callbacks(glfw_handle_);

  // Default logo
  set_icon_dir(DATA_FOLDER / "logo" / "png");

#if defined(IMP_PLATFORM_WINDOWS)
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

  if (params.mode == WindowMode::windowed && !is_flag_set(params.flags, WindowFlags::hidden))
    glfwShowWindow(glfw_handle_);
}

void Window::open_fullscreen_(const InitializeParams& params) {
#if defined(IMP_PLATFORM_WINDOWS)
  GLFWmonitor* monitor = get_monitor_(params.monitor_num);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  if (params.mode == WindowMode::borderless) {
    // borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    // This seems to cause flickering
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), nullptr, nullptr);
  } else
    glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);

  if (!glfw_handle_) {
    const char* description;
    int code = glfwGetError(&description);
    IMP_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  IMP_LOG_DEBUG("Created GLFW window");

  if (params.mode == WindowMode::borderless) {
    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    glfwSetWindowPos(glfw_handle_, base_x, base_y);
  }
#elif defined(IMP_PLATFORM_LINUX)
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

  if (params.mode == WindowMode::borderless) {
    //    borderless = true;

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  }

  glfw_handle_ = glfwCreateWindow(mode->width, mode->height, params.title.c_str(), monitor, nullptr);
  if (!glfw_handle_) {
    const char* description;
    int code = glfwGetError(&description);
    IMP_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  } else
    IMP_LOG_DEBUG("Created GLFW window");
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
    IMP_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  IMP_LOG_DEBUG("Created GLFW window");

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

  if (should_close()) {
    Hermes::send_nowait<E_GlfwWindowClose>(glfw_handle_);
  }
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
  overlay_.size[0] = p.width;
  overlay_.size[1] = p.height;
}

void Window::r_glfw_framebuffer_size_(const E_GlfwFramebufferSize& p) {}

void Window::r_glfw_window_content_scale_(const E_GlfwWindowContentScale& p) {}

void Window::r_glfw_window_pos_(const E_GlfwWindowPos& p) {
  pos_.x = p.xpos;
  pos_.y = p.ypos;
  overlay_.pos[0] = p.xpos;
  overlay_.pos[1] = p.ypos;
  overlay_.current_monitor = detect_monitor_num();
}

void Window::r_glfw_window_iconify_(const E_GlfwWindowIconify& p) {}

void Window::r_glfw_window_maximize_(const E_GlfwWindowMaximize& p) {}

void Window::r_glfw_window_focus_(const E_GlfwWindowFocus& p) {}

void Window::r_glfw_window_refresh_(const E_GlfwWindowRefresh& p) {}

void Window::r_glfw_monitor_(const E_GlfwMonitor& p) {
  monitors_ = glfwGetMonitors(&monitor_count_);
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
#if defined(IMP_PLATFORM_WINDOWS)
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
    IMP_LOG_WARN("Failed to read Windows app theme from registry, error code {}", code);
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

  if (message == WM_SETTINGCHANGE && hwnd == window->win32_hwnd_)
    set_win32_titlebar_color_(hwnd);

  return CallWindowProc(window->win32_saved_WndProc_, hwnd, message, wParam, lParam);
}
#endif
} // namespace imp
