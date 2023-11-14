#ifndef BAPHY_CORE_MODULE_WINDOW_HPP
#define BAPHY_CORE_MODULE_WINDOW_HPP

#define GLFW_INCLUDE_NONE
#include "baphy/core/module_mgr.hpp"
#include "baphy/util/platform.hpp"
#include "GLFW/glfw3.h"
#include <mutex>

namespace baphy {
class Window : public Module<Window> {
public:
  Window() : Module() {}

  GLFWwindow* handle() const { return glfw_handle_; }

  int x() const { return pos_.x; }
  int y() const { return pos_.y; }

  int w() const { return size_.x; }
  int h() const { return size_.y; }

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  static std::once_flag initialize_glfw_;

  static GLFWmonitor* get_monitor_(int monitor_num);

  GLFWwindow* glfw_handle_{nullptr};

  glm::ivec2 size_{};
  glm::ivec2 pos_{};

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
} // namespace baphy

BAPHY_PRAISE_HERMES(baphy::Window);

#endif//BAPHY_CORE_MODULE_WINDOW_HPP
