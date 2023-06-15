#include "baphy/core/glfw_callbacks.hpp"

#include "baphy/core/event_bus.hpp"
#include "baphy/core/msgs.hpp"
#include "baphy/util/log.hpp"
#include "imgui.h"

namespace baphy {

void register_glfw_error_callback() {
  glfwSetErrorCallback(internal::error_callback);
}

void register_glfw_callbacks(GLFWwindow *window) {
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

  BAPHY_LOG_DEBUG("Registered GLFW callbacks");
}

namespace internal {

void error_callback(int code, const char *description) {
  BAPHY_LOG_ERROR("GLFW ({}): {}", code, description);
}

void window_close_callback(GLFWwindow *window) {
  EventBus::send_nowait<EGlfwWindowClose>(window);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
  EventBus::send_nowait<EGlfwWindowSize>(window, width, height);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  EventBus::send_nowait<EGlfwFramebufferSize>(window, width, height);
}

void window_content_scale_callback(GLFWwindow *window, float xscale, float yscale) {
  EventBus::send_nowait<EGlfwWindowContentScale>(window, xscale, yscale);
}

void window_pos_callback(GLFWwindow *window, int xpos, int ypos) {
  EventBus::send_nowait<EGlfwWindowPos>(window, xpos, ypos);
}

void window_iconify_callback(GLFWwindow *window, int iconified) {
  EventBus::send_nowait<EGlfwWindowIconify>(window, iconified);
}

void window_maximize_callback(GLFWwindow *window, int maximized) {
  EventBus::send_nowait<EGlfwWindowMaximize>(window, maximized);
}

void window_focus_callback(GLFWwindow *window, int focused) {
  EventBus::send_nowait<EGlfwWindowFocus>(window, focused);
}

void window_refresh_callback(GLFWwindow *window) {
  EventBus::send_nowait<EGlfwWindowRefresh>(window);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (!ImGui::GetIO().WantCaptureKeyboard)
    EventBus::send_nowait<EGlfwKey>(window, key, scancode, action, mods);
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
  if (!ImGui::GetIO().WantCaptureKeyboard)
    EventBus::send_nowait<EGlfwCharacter>(window, codepoint);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!ImGui::GetIO().WantCaptureMouse)
    EventBus::send_nowait<EGlfwCursorPos>(window, xpos, ypos);
}

void cursor_enter_callback(GLFWwindow *window, int entered) {
  if (!ImGui::GetIO().WantCaptureMouse)
    EventBus::send_nowait<EGlfwCursorEnter>(window, entered);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  if (!ImGui::GetIO().WantCaptureMouse)
    EventBus::send_nowait<EGlfwMouseButton>(window, button, action, mods);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (!ImGui::GetIO().WantCaptureMouse)
    EventBus::send_nowait<EGlfwScroll>(window, xoffset, yoffset);
}

void joystick_callback(int jid, int event) {
  EventBus::send_nowait<EGlfwJoystick>(jid, event);
}

void drop_callback(GLFWwindow *window, int count, const char **paths) {
  EventBus::send_nowait<EGlfwDrop>(window, count, paths);
}

void monitor_callback(GLFWmonitor *monitor, int event) {
  EventBus::send_nowait<EGlfwMonitor>(monitor, event);
}

} // namespace internal
} // namespace baphy
