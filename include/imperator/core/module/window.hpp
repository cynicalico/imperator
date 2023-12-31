#ifndef IMPERATOR_CORE_MODULE_WINDOW_HPP
#define IMPERATOR_CORE_MODULE_WINDOW_HPP

#define GLFW_INCLUDE_NONE
#include "imperator/core/module_mgr.hpp"
#include "imperator/util/module/debug_overlay.hpp"
#include "imperator/util/platform.hpp"
#include "GLFW/glfw3.h"
#include "glm/mat4x4.hpp"
#include <mutex>

namespace imp {
class Window : public Module<Window> {
public:
  std::shared_ptr<DebugOverlay> debug_overlay{nullptr};

  Window() : Module({EPI<DebugOverlay>::name}) {}

  GLFWwindow* handle() const { return glfw_handle_; }

  bool should_close() const;

  bool resizable() const;
  bool decorated() const;
  bool auto_iconify() const;
  bool floating() const;
  bool focus_on_show() const;

  int x() const { return pos_.x; }
  int y() const { return pos_.y; }

  int w() const { return size_.x; }
  int h() const { return size_.y; }

  std::string title() const { return title_; }

  glm::mat4 projection_matrix() const;

  void set_should_close(bool should_close);

  void set_resizable(bool resizable);
  void set_decorated(bool decorated);
  void set_auto_iconify(bool auto_iconify);
  void set_floating(bool floating);
  void set_focus_on_show(bool focus_on_show);

  void set_pos(int x, int y);
  void set_x(int x);
  void set_y(int y);

  void set_size(int w, int h);
  void set_w(int w);
  void set_h(int h);

  void set_title(const std::string& title);

  void set_icon(const std::vector<std::filesystem::path>& paths);
  void set_icon(const std::filesystem::path& path);
  void set_icon_dir(const std::filesystem::path& dir);

  void set_monitor(
    WindowMode mode,
    int monitor_num,
    int x, int y,
    int w, int h
  );

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  static std::once_flag initialize_glfw_;

  int detect_monitor_num() const;
  static GLFWmonitor* get_monitor_(int monitor_num);
  GLFWmonitor** monitors_{};
  int monitor_count_{0};

  GLFWwindow* glfw_handle_{nullptr};

  WindowMode mode_{WindowMode::windowed};
  glm::ivec2 size_{};
  glm::ivec2 pos_{};
  std::string title_{};

  struct {
    std::vector<std::pair<WindowMode, std::string>> modes = {
      {WindowMode::windowed, "Windowed"},
      {WindowMode::fullscreen, "Fullscreen"},
      {WindowMode::borderless, "Borderless"}
    };
    std::size_t current_mode;

    std::size_t current_monitor;

    bool is_pos_dirty{false};
    int pos[2];

    bool is_size_dirty{false};
    int size[2];
  } overlay_{};

  void open_(const InitializeParams& params);
  void open_fullscreen_(const InitializeParams& params);
  void open_windowed_(const InitializeParams& params);

  void r_end_frame_(const E_EndFrame& p);
  void r_update_(const E_Update& p);

  void r_glfw_window_close_(const E_GlfwWindowClose& p);
  void r_glfw_window_size_(const E_GlfwWindowSize& p);
  void r_glfw_framebuffer_size_(const E_GlfwFramebufferSize& p);
  void r_glfw_window_content_scale_(const E_GlfwWindowContentScale& p);
  void r_glfw_window_pos_(const E_GlfwWindowPos& p);
  void r_glfw_window_iconify_(const E_GlfwWindowIconify& p);
  void r_glfw_window_maximize_(const E_GlfwWindowMaximize& p);
  void r_glfw_window_focus_(const E_GlfwWindowFocus& p);
  void r_glfw_window_refresh_(const E_GlfwWindowRefresh& p);
  void r_glfw_monitor_(const E_GlfwMonitor& p);
};
} // namespace imp

IMPERATOR_PRAISE_HERMES(imp::Window);

#endif//IMPERATOR_CORE_MODULE_WINDOW_HPP
