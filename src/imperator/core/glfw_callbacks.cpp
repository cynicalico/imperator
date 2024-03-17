#include "imperator/core/glfw_callbacks.h"

#include "imperator/core/engine.h"
#include "imperator/util/log.h"

namespace imp {
namespace internal {
static void* user_pointer{nullptr};
} // namespace internal

void register_glfw_error_callback() {
  glfwSetErrorCallback(internal::error_callback);
}

void register_glfw_callbacks(GLFWwindow* window) {
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

  IMPERATOR_LOG_DEBUG("Registered GLFW callbacks");
}

void set_glfw_user_pointer(GLFWwindow* window, void* p) {
  glfwSetWindowUserPointer(window, p);
}

void clear_glfw_user_pointer(GLFWwindow* window) {
  glfwSetWindowUserPointer(window, nullptr);
}

void set_global_user_pointer(void* p) {
  internal::user_pointer = p;
}

void clear_global_user_pointer() {
  internal::user_pointer = nullptr;
}

namespace internal {
void error_callback(int code, const char* description) {
  IMPERATOR_LOG_ERROR("GLFW ({}): {}", code, description);
}

void window_close_callback(GLFWwindow* window) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowClose>(window);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowSize>(window, width, height);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwFramebufferSize>(window, width, height);
}

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowContentScale>(window, xscale, yscale);
}

void window_pos_callback(GLFWwindow* window, int xpos, int ypos) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowPos>(window, xpos, ypos);
}

void window_iconify_callback(GLFWwindow* window, int iconified) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowIconify>(window, iconified);
}

void window_maximize_callback(GLFWwindow* window, int maximized) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowMaximize>(window, maximized);
}

void window_focus_callback(GLFWwindow* window, int focused) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowFocus>(window, focused);
}

void window_refresh_callback(GLFWwindow* window) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwWindowRefresh>(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwKey>(window, key, scancode, action, mods);
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwCharacter>(window, codepoint);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwCursorPos>(window, xpos, ypos);
}

void cursor_enter_callback(GLFWwindow* window, int entered) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwCursorEnter>(window, entered);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwMouseButton>(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwScroll>(window, xoffset, yoffset);
}

void joystick_callback(int jid, int event) {
  const auto engine = static_cast<Engine*>(user_pointer);
  engine->send_event<E_GlfwJoystick>(jid, event);
}

void drop_callback(GLFWwindow* window, int count, const char** paths) {
  const auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->send_event<E_GlfwDrop>(window, count, paths);
}

void monitor_callback(GLFWmonitor* monitor, int event) {
  const auto engine = static_cast<Engine*>(user_pointer);
  engine->send_event<E_GlfwMonitor>(monitor, event);
}
} // namespace internal
} // namespace imp
