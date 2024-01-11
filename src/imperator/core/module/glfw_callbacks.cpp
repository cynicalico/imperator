#include "imperator/core/module/glfw_callbacks.hpp"

#include "imperator/core/hermes.hpp"
#include "imperator/util/log.hpp"
#include "imgui.h"

namespace imp {
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

  IMP_LOG_DEBUG("Registered GLFW callbacks");
}

void set_ignore_imgui_capture(const std::vector<int>& keys, int action) {
  if (action == GLFW_PRESS) {
    internal::ignore_imgui_capture_pressed().emplace_back(keys);
  } else if (action == GLFW_RELEASE) {
    internal::ignore_imgui_capture_released().emplace_back(keys);
  }
}

namespace internal {
std::vector<std::vector<int>>& ignore_imgui_capture_pressed() {
  static std::vector<std::vector<int>> v{};
  return v;
}

std::vector<std::vector<int>>& ignore_imgui_capture_released() {
  static std::vector<std::vector<int>> v{};
  return v;
}

bool check_ignore_imgui_capture(int key, int action) {
  std::vector<std::vector<int>> v;
  if (action == GLFW_PRESS) {
    v = ignore_imgui_capture_pressed();
  } else if (action == GLFW_RELEASE) {
    v = ignore_imgui_capture_released();
  } else {
    return false;
  }

  for (auto it = v.begin(); it != v.end(); ++it) {
    if (std::ranges::any_of(*it, [&](int i) { return i == key; })) {
      v.erase(it);
      return true;
    }
  }
  return false;
}

void error_callback(int code, const char* description) {
  IMP_LOG_ERROR("GLFW ({}): {}", code, description);
}

void window_close_callback(GLFWwindow* window) {
  Hermes::send<E_GlfwWindowClose>(window);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
  Hermes::send<E_GlfwWindowSize>(window, width, height);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  Hermes::send<E_GlfwFramebufferSize>(window, width, height);
}

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale) {
  Hermes::send<E_GlfwWindowContentScale>(window, xscale, yscale);
}

void window_pos_callback(GLFWwindow* window, int xpos, int ypos) {
  Hermes::send<E_GlfwWindowPos>(window, xpos, ypos);
}

void window_iconify_callback(GLFWwindow* window, int iconified) {
  Hermes::send<E_GlfwWindowIconify>(window, iconified);
}

void window_maximize_callback(GLFWwindow* window, int maximized) {
  Hermes::send<E_GlfwWindowMaximize>(window, maximized);
}

void window_focus_callback(GLFWwindow* window, int focused) {
  Hermes::send<E_GlfwWindowFocus>(window, focused);
}

void window_refresh_callback(GLFWwindow* window) {
  Hermes::send<E_GlfwWindowRefresh>(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  bool should_send = true;
  if (ImGui::GetIO().WantCaptureKeyboard) {
    should_send = check_ignore_imgui_capture(key, action);
  }

  if (should_send) {
    Hermes::send<E_GlfwKey>(window, key, scancode, action, mods);
  }
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
  if (!ImGui::GetIO().WantCaptureKeyboard) {
    Hermes::send<E_GlfwCharacter>(window, codepoint);
  }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    Hermes::send<E_GlfwCursorPos>(window, xpos, ypos);
  }
}

void cursor_enter_callback(GLFWwindow* window, int entered) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    Hermes::send<E_GlfwCursorEnter>(window, entered);
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    Hermes::send<E_GlfwMouseButton>(window, button, action, mods);
  }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    Hermes::send<E_GlfwScroll>(window, xoffset, yoffset);
  }
}

void joystick_callback(int jid, int event) {
  Hermes::send<E_GlfwJoystick>(jid, event);
}

void drop_callback(GLFWwindow* window, int count, const char** paths) {
  Hermes::send<E_GlfwDrop>(window, count, paths);
}

void monitor_callback(GLFWmonitor* monitor, int event) {
  Hermes::send<E_GlfwMonitor>(monitor, event);
}
} // namespace internal
} // namespace imp
