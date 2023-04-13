#include "myco/util/log.hpp"

#include "myco/core/glfw_callbacks.hpp"
#include "myco/core/scheduler.hpp"

namespace myco {

void register_glfw_callbacks(GLFWwindow *window) {
  glfwSetErrorCallback(internal::error_callback);

  glfwSetWindowCloseCallback(window, internal::window_close_callback);
  glfwSetWindowSizeCallback(window, internal::window_size_callback);
  glfwSetFramebufferSizeCallback(window, internal::framebuffer_size_callback);
  glfwSetWindowContentScaleCallback(window, internal::window_content_scale_callback);
  glfwSetWindowPosCallback(window, internal::window_pos_callback);
  glfwSetWindowIconifyCallback(window, internal::window_iconify_callback);
  glfwSetWindowMaximizeCallback(window, internal::window_maximize_callback);
  glfwSetWindowFocusCallback(window, internal::window_focus_callback);
  glfwSetWindowRefreshCallback(window, internal::window_refresh_callback);

  glfwSetKeyCallback(window, internal::key_callback);
  glfwSetCharCallback(window, internal::character_callback);
  glfwSetCursorPosCallback(window, internal::cursor_position_callback);
  glfwSetCursorEnterCallback(window, internal::cursor_enter_callback);
  glfwSetMouseButtonCallback(window, internal::mouse_button_callback);
  glfwSetScrollCallback(window, internal::scroll_callback);
  glfwSetJoystickCallback(internal::joystick_callback);
  glfwSetDropCallback(window, internal::drop_callback);

  glfwSetMonitorCallback(internal::monitor_callback);

  MYCO_LOG_DEBUG("Registered GLFW callbacks");
}

namespace internal {

void error_callback(int code, const char *description) {
  MYCO_LOG_ERROR("GLFW ({}): {}", code, description);
}

void window_close_callback(GLFWwindow *window) {
  Scheduler::send_nowait<GlfwWindowClose>(window);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
  Scheduler::send_nowait<GlfwWindowSize>(window, width, height);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  Scheduler::send_nowait<GlfwFramebufferSize>(window, width, height);
}

void window_content_scale_callback(GLFWwindow *window, float xscale, float yscale) {
  Scheduler::send_nowait<GlfwWindowContentScale>(window, xscale, yscale);
}

void window_pos_callback(GLFWwindow *window, int xpos, int ypos) {
  Scheduler::send_nowait<GlfwWindowPos>(window, xpos, ypos);
}

void window_iconify_callback(GLFWwindow *window, int iconified) {
  Scheduler::send_nowait<GlfwWindowIconify>(window, iconified);
}

void window_maximize_callback(GLFWwindow *window, int maximized) {
  Scheduler::send_nowait<GlfwWindowMaximize>(window, maximized);
}

void window_focus_callback(GLFWwindow *window, int focused) {
  Scheduler::send_nowait<GlfwWindowFocus>(window, focused);
}

void window_refresh_callback(GLFWwindow *window) {
  Scheduler::send_nowait<GlfwWindowRefresh>(window);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//  if (!ImGui::GetIO().WantCaptureKeyboard)
  Scheduler::send_nowait<GlfwKey>(window, key, scancode, action, mods);
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
//  if (!ImGui::GetIO().WantCaptureKeyboard)
  Scheduler::send_nowait<GlfwCharacter>(window, codepoint);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
//  if (!ImGui::GetIO().WantCaptureMouse)
  Scheduler::send_nowait<GlfwCursorPos>(window, xpos, ypos);
}

void cursor_enter_callback(GLFWwindow *window, int entered) {
//  if (!ImGui::GetIO().WantCaptureMouse)
  Scheduler::send_nowait<GlfwCursorEnter>(window, entered);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
//  if (!ImGui::GetIO().WantCaptureMouse)
  Scheduler::send_nowait<GlfwMouseButton>(window, button, action, mods);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
//  if (!ImGui::GetIO().WantCaptureMouse)
  Scheduler::send_nowait<GlfwScroll>(window, xoffset, yoffset);
}

void joystick_callback(int jid, int event) {
  Scheduler::send_nowait<GlfwJoystick>(jid, event);
}

void drop_callback(GLFWwindow *window, int count, const char **paths) {
  Scheduler::send_nowait<GlfwDrop>(window, count, paths);
}

void monitor_callback(GLFWmonitor *monitor, int event) {
  Scheduler::send_nowait<GlfwMonitor>(monitor, event);
}

} // namespace internal
} // namespace myco
