#ifndef BAPHY_CORE_MODULE_WINDOW_HPP
#define BAPHY_CORE_MODULE_WINDOW_HPP

#include "baphy/core/module/module.hpp"
#include "baphy/util/platform.hpp"
#include "GLFW/glfw3.h"
#include <mutex>

#if defined(BAPHY_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace baphy {

class Window : public Module<Window> {
public:
  Window() : Module<Window>() {}

  ~Window() override = default;

  GLFWwindow *handle();

  void open(const WindowOpenParams &params);
  WindowOpenParams open_params() const;

  int w() const;
  int h() const;
  glm::ivec2 size() const;
  bool should_close() const;

  void set_w(int w);
  void set_h(int h);
  void set_size(int w, int h);
  void set_should_close(bool v);

  void swap() const;

private:
  static std::once_flag initialize_glfw_;
  static std::once_flag terminate_glfw_;

  GLFWwindow *glfw_handle_{nullptr};
  WindowOpenParams open_params_{};

  GLFWmonitor *get_monitor_(int monitor_num);
  void open_fullscreen_(const WindowOpenParams &params);
  void open_windowed_(const WindowOpenParams &params);

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
  void e_start_frame_(const EStartFrame &e);
  void e_end_frame_(const EEndFrame &e);

  void e_glfw_window_close_(const EGlfwWindowClose &e);

#if defined(BAPHY_PLATFORM_WINDOWS)
  HWND win32_hwnd_{nullptr};
  WNDPROC win32_saved_WndProc_{nullptr};
  bool win32_force_light_mode_{false};
  bool win32_force_dark_mode_{false};

  static void set_win32_titlebar_color_(HWND hwnd);
  static LRESULT CALLBACK WndProc_(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Window);

#endif//BAPHY_CORE_MODULE_WINDOW_HPP
