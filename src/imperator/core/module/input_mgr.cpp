#include "imperator/core/module/input_mgr.hpp"

#include "imperator/util/time.hpp"
#include <ranges>

namespace imp {
std::once_flag InputMgr::initialize_glfw_action_maps_;

std::vector<int> InputMgr::all_glfw_keys_{
  GLFW_KEY_UNKNOWN,GLFW_KEY_SPACE,GLFW_KEY_APOSTROPHE,GLFW_KEY_COMMA,GLFW_KEY_MINUS,GLFW_KEY_PERIOD,GLFW_KEY_SLASH,
  GLFW_KEY_0,GLFW_KEY_1,GLFW_KEY_2,GLFW_KEY_3,GLFW_KEY_4,GLFW_KEY_5,GLFW_KEY_6,GLFW_KEY_7,GLFW_KEY_8,GLFW_KEY_9,
  GLFW_KEY_SEMICOLON,GLFW_KEY_EQUAL,GLFW_KEY_A,GLFW_KEY_B,GLFW_KEY_C,GLFW_KEY_D,GLFW_KEY_E,GLFW_KEY_F,GLFW_KEY_G,
  GLFW_KEY_H,GLFW_KEY_I,GLFW_KEY_J,GLFW_KEY_K,GLFW_KEY_L,GLFW_KEY_M,GLFW_KEY_N,GLFW_KEY_O,GLFW_KEY_P,GLFW_KEY_Q,
  GLFW_KEY_R,GLFW_KEY_S,GLFW_KEY_T,GLFW_KEY_U,GLFW_KEY_V,GLFW_KEY_W,GLFW_KEY_X,GLFW_KEY_Y,GLFW_KEY_Z,
  GLFW_KEY_LEFT_BRACKET,GLFW_KEY_BACKSLASH,GLFW_KEY_RIGHT_BRACKET,GLFW_KEY_GRAVE_ACCENT,GLFW_KEY_WORLD_1,
  GLFW_KEY_WORLD_2,GLFW_KEY_ESCAPE,GLFW_KEY_ENTER,GLFW_KEY_TAB,GLFW_KEY_BACKSPACE,GLFW_KEY_INSERT,GLFW_KEY_DELETE,
  GLFW_KEY_RIGHT,GLFW_KEY_LEFT,GLFW_KEY_DOWN,GLFW_KEY_UP,GLFW_KEY_PAGE_UP,GLFW_KEY_PAGE_DOWN,GLFW_KEY_HOME,GLFW_KEY_END,
  GLFW_KEY_CAPS_LOCK,GLFW_KEY_SCROLL_LOCK,GLFW_KEY_NUM_LOCK,GLFW_KEY_PRINT_SCREEN,GLFW_KEY_PAUSE,GLFW_KEY_F1,
  GLFW_KEY_F2,GLFW_KEY_F3,GLFW_KEY_F4,GLFW_KEY_F5,GLFW_KEY_F6,GLFW_KEY_F7,GLFW_KEY_F8,GLFW_KEY_F9,GLFW_KEY_F10,
  GLFW_KEY_F11,GLFW_KEY_F12,GLFW_KEY_F13,GLFW_KEY_F14,GLFW_KEY_F15,GLFW_KEY_F16,GLFW_KEY_F17,GLFW_KEY_F18,GLFW_KEY_F19,
  GLFW_KEY_F20,GLFW_KEY_F21,GLFW_KEY_F22,GLFW_KEY_F23,GLFW_KEY_F24,GLFW_KEY_F25,GLFW_KEY_KP_0,GLFW_KEY_KP_1,
  GLFW_KEY_KP_2,GLFW_KEY_KP_3,GLFW_KEY_KP_4,GLFW_KEY_KP_5,GLFW_KEY_KP_6,GLFW_KEY_KP_7,GLFW_KEY_KP_8,GLFW_KEY_KP_9,
  GLFW_KEY_KP_DECIMAL,GLFW_KEY_KP_DIVIDE,GLFW_KEY_KP_MULTIPLY,GLFW_KEY_KP_SUBTRACT,GLFW_KEY_KP_ADD,GLFW_KEY_KP_ENTER,
  GLFW_KEY_KP_EQUAL,GLFW_KEY_LEFT_SHIFT,GLFW_KEY_LEFT_CONTROL,GLFW_KEY_LEFT_ALT,GLFW_KEY_LEFT_SUPER,
  GLFW_KEY_RIGHT_SHIFT,GLFW_KEY_RIGHT_CONTROL,GLFW_KEY_RIGHT_ALT,GLFW_KEY_RIGHT_SUPER,GLFW_KEY_MENU,
};
std::vector<std::string> InputMgr::all_str_keys_{
  "unknown", "space", "apostrophe", "comma", "minus", "period", "slash", "0", "1", "2", "3", "4", "5", "6", "7", "8",
  "9", "semicolon", "equal", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r",
  "s", "t", "u", "v", "w", "x", "y", "z", "left_bracket", "backslash", "right_bracket", "grave_accent", "world_1",
  "world_2", "escape", "enter", "tab", "backspace", "insert", "delete", "right", "left", "down", "up", "page_up",
  "page_down", "home", "end", "caps_lock", "scroll_lock", "num_lock", "print_screen", "pause", "f1", "f2", "f3", "f4",
  "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21",
  "f22", "f23", "f24", "f25", "kp_0", "kp_1", "kp_2", "kp_3", "kp_4", "kp_5", "kp_6", "kp_7", "kp_8", "kp_9",
  "kp_decimal", "kp_divide", "kp_multiply", "kp_subtract", "kp_add", "kp_enter", "kp_equal", "left_shift",
  "left_control", "left_alt", "left_super", "right_shift", "right_control", "right_alt", "right_super", "menu"
};
std::unordered_map<int, std::string> InputMgr::glfw_key_to_str_{};
std::unordered_map<std::string, int> InputMgr::str_to_glfw_key_{};

std::vector<int> InputMgr::all_glfw_buttons_{
  GLFW_MOUSE_BUTTON_1,GLFW_MOUSE_BUTTON_2,GLFW_MOUSE_BUTTON_3,GLFW_MOUSE_BUTTON_4,GLFW_MOUSE_BUTTON_5,
  GLFW_MOUSE_BUTTON_6,GLFW_MOUSE_BUTTON_7,GLFW_MOUSE_BUTTON_8
};
std::vector<std::string> InputMgr::all_str_buttons_{
  "mb_left", "mb_right", "mb_middle", "mb_4", "mb_5", "mb_6", "mb_7", "mb_8"
};
std::unordered_map<int, std::string> InputMgr::glfw_button_to_str_{};
std::unordered_map<std::string, int> InputMgr::str_to_glfw_button_{};

void InputMgr::bind(const std::string& name, const std::string& action) {
  if (auto it = bindings_.find(name); it == bindings_.end())
    bindings_[name] = {};
  bindings_[name].emplace_back(action);
}

bool InputMgr::pressed(const std::string& name, const Mods& mods) {
  int mods_check = unwrap(mods);
  return std::ranges::any_of(bindings_[name], [&](const auto& b) {
    return state_[b].pressed && !state_[b].last_pressed && (state_[b].mods & mods_check) == mods_check;
  });
}

void InputMgr::r_initialize_(const E_Initialize& p) {
  std::call_once(initialize_glfw_action_maps_, [&] {
    for (std::size_t i = 0; i < all_glfw_keys_.size(); ++i) {
      glfw_key_to_str_[all_glfw_keys_[i]] = all_str_keys_[i];
      str_to_glfw_key_[all_str_keys_[i]] = all_glfw_keys_[i];
    }
    for (std::size_t i = 0; i < all_glfw_buttons_.size(); ++i) {
      glfw_button_to_str_[all_glfw_buttons_[i]] = all_str_buttons_[i];
      str_to_glfw_button_[all_str_buttons_[i]] = all_glfw_buttons_[i];
    }
  });

  for (const auto& a: all_str_keys_) {
    bind(a, a);
  }
  for (const auto& a: all_str_buttons_) {
    bind(a, a);
  }

  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });
  Hermes::sub<E_GlfwKey>(module_name, [&](const auto& p) { r_glfw_key_(p); });
  Hermes::sub<E_GlfwCharacter>(module_name, [&](const auto& p) { r_glfw_character_(p); });
  Hermes::sub<E_GlfwCursorPos>(module_name, [&](const auto& p) { r_glfw_cursor_pos_(p); });
  Hermes::sub<E_GlfwCursorEnter>(module_name, [&](const auto& p) { r_glfw_cursor_enter_(p); });
  Hermes::sub<E_GlfwMouseButton>(module_name, [&](const auto& p) { r_glfw_mouse_button_(p); });
  Hermes::sub<E_GlfwScroll>(module_name, [&](const auto& p) { r_glfw_scroll_(p); });

  Module::r_initialize_(p);
}

void InputMgr::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}

void InputMgr::r_update_(const E_Update& p) {
  for (auto& s: state_ | std::views::values) {
    s.last_pressed = s.pressed;
  }

  Hermes::poll<E_GlfwKey>(module_name);
  Hermes::poll<E_GlfwCharacter>(module_name);
  Hermes::poll<E_GlfwCursorPos>(module_name);
  Hermes::poll<E_GlfwCursorEnter>(module_name);
  Hermes::poll<E_GlfwMouseButton>(module_name);
  Hermes::poll<E_GlfwScroll>(module_name);

  while (!action_queue_.empty()) {
    auto [action, pressed, mods, time] = action_queue_.front();
    action_queue_.pop();

    state_[action].pressed = pressed;
    state_[action].mods = mods;
    if (pressed) {
      state_[action].press_time = time;
    } else {
      state_[action].release_time = time;
    }
  }
}

void InputMgr::r_glfw_key_(const E_GlfwKey& p) {
  auto action = glfw_key_to_str_[p.key];

  if (p.action == GLFW_PRESS) {
    action_queue_.emplace(action, true, p.mods, time_nsec());
  } else if (p.action == GLFW_RELEASE) {
    action_queue_.emplace(action, false, p.mods, time_nsec());
  }
}

void InputMgr::r_glfw_character_(const E_GlfwCharacter& p) {}

void InputMgr::r_glfw_cursor_pos_(const E_GlfwCursorPos& p) {}

void InputMgr::r_glfw_cursor_enter_(const E_GlfwCursorEnter& p) {}

void InputMgr::r_glfw_mouse_button_(const E_GlfwMouseButton& p) {
  auto action = glfw_button_to_str_[p.button];

  if (p.action == GLFW_PRESS) {
    action_queue_.emplace(action, true, p.mods, time_nsec());
  } else if (p.action == GLFW_RELEASE) {
    action_queue_.emplace(action, false, p.mods, time_nsec());
  }
}

void InputMgr::r_glfw_scroll_(const E_GlfwScroll& p) {}
} // namespace imp
