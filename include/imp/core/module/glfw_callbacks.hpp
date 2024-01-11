#ifndef IMP_CORE_MODULE_GLFW_CALLBACKS_HPP
#define IMP_CORE_MODULE_GLFW_CALLBACKS_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <vector>

namespace imp {
void register_glfw_error_callback();

void register_glfw_callbacks(GLFWwindow* window);

/* Propagate key actions to Hermes (one time) even when imgui says to ignore it
 *
 * This handles the situation where a key is pressed, and immediately an imgui window
 * is opened and focused, which eats the release action, which desyncs the real state
 * of the keyboard and the inputmgr state
 *
 * It is effectively saying
 *  "I am expecting an action to occur in the future, do not let imgui eat it"
 *
 * The vector of keys should be retrieved from InputMgr::get_glfw_actions(...).
 *
 * You can also do the action on key/button *release* instead, which means you
 * can ignore this entirely and it won't be an issue at all.
 */
void set_ignore_imgui_capture(const std::vector<int>& keys, int action);

namespace internal {
std::vector<std::vector<int>>& ignore_imgui_capture_pressed();
std::vector<std::vector<int>>& ignore_imgui_capture_released();
bool check_ignore_imgui_capture(int key, int action);

void error_callback(int code, const char* description);

void window_close_callback(GLFWwindow* window);

void window_size_callback(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale);

void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

void window_iconify_callback(GLFWwindow* window, int iconified);

void window_maximize_callback(GLFWwindow* window, int maximized);

void window_focus_callback(GLFWwindow* window, int focused);

void window_refresh_callback(GLFWwindow* window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void character_callback(GLFWwindow* window, unsigned int codepoint);

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void cursor_enter_callback(GLFWwindow* window, int entered);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void joystick_callback(int jid, int event);

void drop_callback(GLFWwindow* window, int count, const char** paths);

void monitor_callback(GLFWmonitor* monitor, int event);
} // namespace internal
} // namespace imp

#endif//IMP_CORE_MODULE_GLFW_CALLBACKS_HPP
