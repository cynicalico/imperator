#include "baphy/core/module/input_mgr.hpp"

#include "baphy/core/glfw_callbacks.hpp"
#include "baphy/util/time.hpp"

namespace baphy {

double InputMgr::mouse_x() {
  return 0;
}

double InputMgr::mouse_y() {
  return 0;
}

double InputMgr::mouse_px() {
  return 0;
}

double InputMgr::mouse_py() {
  return 0;
}

double InputMgr::mouse_dx() {
  return 0;
}

double InputMgr::mouse_dy() {
  return 0;
}

bool InputMgr::pressed(const std::string &binding) {
  return state_[binding].pressed && !state_[binding].last_pressed;
}

bool InputMgr::released(const std::string &binding) {
  return !state_[binding].pressed && state_[binding].last_pressed;
}

bool InputMgr::down(const std::string &binding, double interval, double delay) {
  if (interval <= 0 && delay <= 0)
    return state_[binding].pressed;

  return false;
}

std::string InputMgr::glfw_key_to_str_(int key) {
  static std::unordered_map<int, std::string> mapping{
      {GLFW_KEY_UNKNOWN,       "unknown"},
      {GLFW_KEY_SPACE,         "space"},
      {GLFW_KEY_APOSTROPHE,    "apostrophe"},
      {GLFW_KEY_COMMA,         "comma"},
      {GLFW_KEY_MINUS,         "minus"},
      {GLFW_KEY_PERIOD,        "period"},
      {GLFW_KEY_SLASH,         "slash"},
      {GLFW_KEY_0,             "0"},
      {GLFW_KEY_1,             "1"},
      {GLFW_KEY_2,             "2"},
      {GLFW_KEY_3,             "3"},
      {GLFW_KEY_4,             "4"},
      {GLFW_KEY_5,             "5"},
      {GLFW_KEY_6,             "6"},
      {GLFW_KEY_7,             "7"},
      {GLFW_KEY_8,             "8"},
      {GLFW_KEY_9,             "9"},
      {GLFW_KEY_SEMICOLON,     "semicolon"},
      {GLFW_KEY_EQUAL,         "equal"},
      {GLFW_KEY_A,             "a"},
      {GLFW_KEY_B,             "b"},
      {GLFW_KEY_C,             "c"},
      {GLFW_KEY_D,             "d"},
      {GLFW_KEY_E,             "e"},
      {GLFW_KEY_F,             "f"},
      {GLFW_KEY_G,             "g"},
      {GLFW_KEY_H,             "h"},
      {GLFW_KEY_I,             "i"},
      {GLFW_KEY_J,             "j"},
      {GLFW_KEY_K,             "k"},
      {GLFW_KEY_L,             "l"},
      {GLFW_KEY_M,             "m"},
      {GLFW_KEY_N,             "n"},
      {GLFW_KEY_O,             "o"},
      {GLFW_KEY_P,             "p"},
      {GLFW_KEY_Q,             "q"},
      {GLFW_KEY_R,             "r"},
      {GLFW_KEY_S,             "s"},
      {GLFW_KEY_T,             "t"},
      {GLFW_KEY_U,             "u"},
      {GLFW_KEY_V,             "v"},
      {GLFW_KEY_W,             "w"},
      {GLFW_KEY_X,             "x"},
      {GLFW_KEY_Y,             "y"},
      {GLFW_KEY_Z,             "z"},
      {GLFW_KEY_LEFT_BRACKET,  "left_bracket"},
      {GLFW_KEY_BACKSLASH,     "backslash"},
      {GLFW_KEY_RIGHT_BRACKET, "right_bracket"},
      {GLFW_KEY_GRAVE_ACCENT,  "grave_accent"},
      {GLFW_KEY_WORLD_1,       "world_1"},
      {GLFW_KEY_WORLD_2,       "world_2"},
      {GLFW_KEY_ESCAPE,        "escape"},
      {GLFW_KEY_ENTER,         "enter"},
      {GLFW_KEY_TAB,           "tab"},
      {GLFW_KEY_BACKSPACE,     "backspace"},
      {GLFW_KEY_INSERT,        "insert"},
      {GLFW_KEY_DELETE,        "delete"},
      {GLFW_KEY_RIGHT,         "right"},
      {GLFW_KEY_LEFT,          "left"},
      {GLFW_KEY_DOWN,          "down"},
      {GLFW_KEY_UP,            "up"},
      {GLFW_KEY_PAGE_UP,       "page_up"},
      {GLFW_KEY_PAGE_DOWN,     "page_down"},
      {GLFW_KEY_HOME,          "home"},
      {GLFW_KEY_END,           "end"},
      {GLFW_KEY_CAPS_LOCK,     "caps_lock"},
      {GLFW_KEY_SCROLL_LOCK,   "scroll_lock"},
      {GLFW_KEY_NUM_LOCK,      "num_lock"},
      {GLFW_KEY_PRINT_SCREEN,  "print_screen"},
      {GLFW_KEY_PAUSE,         "pause"},
      {GLFW_KEY_F1,            "f1"},
      {GLFW_KEY_F2,            "f2"},
      {GLFW_KEY_F3,            "f3"},
      {GLFW_KEY_F4,            "f4"},
      {GLFW_KEY_F5,            "f5"},
      {GLFW_KEY_F6,            "f6"},
      {GLFW_KEY_F7,            "f7"},
      {GLFW_KEY_F8,            "f8"},
      {GLFW_KEY_F9,            "f9"},
      {GLFW_KEY_F10,           "f10"},
      {GLFW_KEY_F11,           "f11"},
      {GLFW_KEY_F12,           "f12"},
      {GLFW_KEY_F13,           "f13"},
      {GLFW_KEY_F14,           "f14"},
      {GLFW_KEY_F15,           "f15"},
      {GLFW_KEY_F16,           "f16"},
      {GLFW_KEY_F17,           "f17"},
      {GLFW_KEY_F18,           "f18"},
      {GLFW_KEY_F19,           "f19"},
      {GLFW_KEY_F20,           "f20"},
      {GLFW_KEY_F21,           "f21"},
      {GLFW_KEY_F22,           "f22"},
      {GLFW_KEY_F23,           "f23"},
      {GLFW_KEY_F24,           "f24"},
      {GLFW_KEY_F25,           "f25"},
      {GLFW_KEY_KP_0,          "kp_0"},
      {GLFW_KEY_KP_1,          "kp_1"},
      {GLFW_KEY_KP_2,          "kp_2"},
      {GLFW_KEY_KP_3,          "kp_3"},
      {GLFW_KEY_KP_4,          "kp_4"},
      {GLFW_KEY_KP_5,          "kp_5"},
      {GLFW_KEY_KP_6,          "kp_6"},
      {GLFW_KEY_KP_7,          "kp_7"},
      {GLFW_KEY_KP_8,          "kp_8"},
      {GLFW_KEY_KP_9,          "kp_9"},
      {GLFW_KEY_KP_DECIMAL,    "kp_decimal"},
      {GLFW_KEY_KP_DIVIDE,     "kp_divide"},
      {GLFW_KEY_KP_MULTIPLY,   "kp_multiply"},
      {GLFW_KEY_KP_SUBTRACT,   "kp_subtract"},
      {GLFW_KEY_KP_ADD,        "kp_add"},
      {GLFW_KEY_KP_ENTER,      "kp_enter"},
      {GLFW_KEY_KP_EQUAL,      "kp_equal"},
      {GLFW_KEY_LEFT_SHIFT,    "left_shift"},
      {GLFW_KEY_LEFT_CONTROL,  "left_control"},
      {GLFW_KEY_LEFT_ALT,      "left_alt"},
      {GLFW_KEY_LEFT_SUPER,    "left_super"},
      {GLFW_KEY_RIGHT_SHIFT,   "right_shift"},
      {GLFW_KEY_RIGHT_CONTROL, "right_control"},
      {GLFW_KEY_RIGHT_ALT,     "right_alt"},
      {GLFW_KEY_RIGHT_SUPER,   "right_super"},
      {GLFW_KEY_MENU,          "menu"}
  };

  return mapping[key];
}

std::string InputMgr::glfw_button_to_str_(int button) {
  static std::unordered_map<int, std::string> mapping{
      {GLFW_MOUSE_BUTTON_1, "mb_left"},
      {GLFW_MOUSE_BUTTON_2, "mb_right"},
      {GLFW_MOUSE_BUTTON_3, "mb_middle"},
      {GLFW_MOUSE_BUTTON_4, "mb_4"},
      {GLFW_MOUSE_BUTTON_5, "mb_5"},
      {GLFW_MOUSE_BUTTON_6, "mb_6"},
      {GLFW_MOUSE_BUTTON_7, "mb_7"},
      {GLFW_MOUSE_BUTTON_8, "mb_8"}
  };

  return mapping[button];
}

const std::unordered_set<std::string> &InputMgr::all_glfw_actions_() {
  static std::unordered_set<std::string> actions = {
      "unknown", "space", "apostrophe", "comma", "minus", "period", "slash", "0", "1", "2", "3", "4", "5", "6", "7",
      "8", "9", "semicolon", "equal", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p",
      "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "left_bracket", "backslash", "right_bracket", "grave_accent",
      "world_1", "world_2", "escape", "enter", "tab", "backspace", "insert", "delete", "right", "left", "down", "up",
      "page_up", "page_down", "home", "end", "caps_lock", "scroll_lock", "num_lock", "print_screen", "pause", "f1",
      "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18",
      "f19", "f20", "f21", "f22", "f23", "f24", "f25", "kp_0", "kp_1", "kp_2", "kp_3", "kp_4", "kp_5", "kp_6", "kp_7",
      "kp_8", "kp_9", "kp_decimal", "kp_divide", "kp_multiply", "kp_subtract", "kp_add", "kp_enter", "kp_equal",
      "left_shift", "left_control", "left_alt", "left_super", "right_shift", "right_control", "right_alt",
      "right_super", "menu", "mb_left", "mb_right", "mb_middle", "mb_4", "mb_5", "mb_6", "mb_7", "mb_8"
  };

  return actions;
}

void InputMgr::e_initialize_(const EInitialize &e) {
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });

  EventBus::sub<EGlfwKey>(module_name, [&](const auto &e) { e_glfw_key_(e); });
  EventBus::sub<EGlfwCursorPos>(module_name, [&](const auto &e) { e_glfw_cursor_pos_(e); });
  EventBus::sub<EGlfwCursorEnter>(module_name, [&](const auto &e) { e_glfw_cursor_enter_(e); });
  EventBus::sub<EGlfwMouseButton>(module_name, [&](const auto &e) { e_glfw_mouse_button_(e); });
  EventBus::sub<EGlfwScroll>(module_name, [&](const auto &e) { e_glfw_scroll_(e); });

  auto window = module_mgr->get<Window>();
  baphy::register_glfw_callbacks(window->handle());

  Module::e_initialize_(e);
}

void InputMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void InputMgr::e_update_(const EUpdate &e) {

}

void InputMgr::e_glfw_key_(const EGlfwKey &e) {

}

void InputMgr::e_glfw_cursor_pos_(const EGlfwCursorPos &e) {

}

void InputMgr::e_glfw_cursor_enter_(const EGlfwCursorEnter &e) {

}

void InputMgr::e_glfw_mouse_button_(const EGlfwMouseButton &e) {

}

void InputMgr::e_glfw_scroll_(const EGlfwScroll &e) {

}

} // namespace baphy
