#ifndef IMPERATOR_MODULE_WINDOW_H
#define IMPERATOR_MODULE_WINDOW_H

#include "imperator/core/glfw_callbacks.h"
#include "imperator/core/glfw_wrap.h"
#include "imperator/module/event_bus.h"
#include "imperator/module/module_mgr.h"

namespace imp {
class Window : public Module<Window> {
public:
  std::shared_ptr<EventBus> event_bus{nullptr};

  Window(ModuleMgr& module_mgr, WindowOpenParams open_params, glm::ivec2 backend_version);
  ~Window() override;

  GLFWwindow* handle() const { return glfw_window_.get(); }

  bool should_close() const;
  void set_should_close(bool v);

  int x() const { return pos_.x; }
  int y() const { return pos_.y; }

  int w() const { return size_.x; }
  int h() const { return size_.y; }

private:
  UniqGLFWwindow glfw_window_;

  WindowMode mode_{WindowMode::windowed};
  glm::ivec2 size_{};
  glm::ivec2 pos_{};
  std::string title_{};

  static GLFWmonitor* get_monitor_(int monitor_num);

  void open_(WindowOpenParams open_params, glm::ivec2 backend_version);
  void open_fullscreen_(WindowOpenParams open_params);
  void open_windowed_(WindowOpenParams open_params);

  void r_update_(const E_Update& p);

  void r_end_frame_(const E_EndFrame& p);

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

IMPERATOR_DECLARE_MODULE(imp::Window);

#endif//IMPERATOR_MODULE_WINDOW_H
