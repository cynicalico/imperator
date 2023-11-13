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

  int w() const;
  int h() const;

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  static std::once_flag initialize_glfw_;

  static GLFWmonitor* get_monitor_(int monitor_num);

  GLFWwindow* glfw_handle_{nullptr};

  void open_(const InitializeParams& params);
  void open_fullscreen_(const InitializeParams& params);
  void open_windowed_(const InitializeParams& params);

  void r_end_frame_(const E_EndFrame& p);
  void r_update_(const E_Update& p);
  void r_glfw_window_close_(const E_GlfwWindowClose& p);
};
} // namespace baphy

BAPHY_PRAISE_HERMES(baphy::Window);

#endif//BAPHY_CORE_MODULE_WINDOW_HPP
