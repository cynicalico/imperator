#include "imperator/core/module/window.hpp"

#include "imperator/core/module/application.hpp"
#include "imperator/core/module/glfw_callbacks.hpp"
#include "imperator/util/io.hpp"
#include "imperator/util/log.hpp"
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
    IMPERATOR_LOG_WARN("No valid icon images found in '{}'", dir.string());
  }

  set_icon(icon_paths);
}

void Window::set_monitor(WindowMode mode, int monitor_num, int x, int y, int w, int h) {
  if (mode == WindowMode::windowed) {
    glfwSetWindowMonitor(glfw_handle_, nullptr, x, y, w, h, 0);
  } else {
    GLFWmonitor* monitor = get_monitor_(monitor_num);
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

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
  }

  mode_ = mode;
}

void Window::r_initialize_(const E_Initialize& p) {
  debug_overlay = module_mgr->get<DebugOverlay>();

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

  monitors_ = glfwGetMonitors(&monitor_count_);
  open_(p.params);
  overlay_.current_mode = mode_ == WindowMode::windowed ? 0 : mode_ == WindowMode::fullscreen ? 1 : 2;
  overlay_.current_monitor = detect_monitor_num();

  debug_overlay->add_tab("Window", [&] {
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
  IMPERATOR_LOG_DEBUG("Terminated GLFW");

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

  if (params.mode == WindowMode::windowed && !is_flag_set(params.flags, WindowFlags::hidden))
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
    IMPERATOR_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  IMPERATOR_LOG_DEBUG("Created GLFW window");

  if (params.mode == WindowMode::borderless) {
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

  if (params.mode == WindowMode::borderless) {
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
} // namespace imp
