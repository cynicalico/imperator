#include "imp/core/module/input_mgr.hpp"

#include "imp/util/time.hpp"
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

InputMgr::InputMgr(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
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

  IMP_HERMES_SUB(E_Update, module_name, r_update_);
  IMP_HERMES_SUB(E_GlfwKey, module_name, r_glfw_key_);
  IMP_HERMES_SUB(E_GlfwCharacter, module_name, r_glfw_character_);
  IMP_HERMES_SUB(E_GlfwCursorPos, module_name, r_glfw_cursor_pos_);
  IMP_HERMES_SUB(E_GlfwCursorEnter, module_name, r_glfw_cursor_enter_);
  IMP_HERMES_SUB(E_GlfwMouseButton, module_name, r_glfw_mouse_button_);
  IMP_HERMES_SUB(E_GlfwScroll, module_name, r_glfw_scroll_);
}

void InputMgr::bind(const std::string& name, const std::string& action) {
  if (auto it = bindings_.find(name); it == bindings_.end())
    bindings_[name] = {};
  bindings_[name].emplace_back(action);
}

std::vector<int> InputMgr::get_glfw_actions(const std::string& name) {
  std::vector<int> glfw_actions{};
  for (const auto& s: bindings_[name]) {
    int action = str_to_glfw_key_[s];
    if (action == 0) {
      action = str_to_glfw_button_[s];
    }
    glfw_actions.emplace_back(action);
  }
  return glfw_actions;
}

bool InputMgr::pressed(const std::string& name, const Mods& mods) {
  int mods_check = unwrap(mods);
  return std::ranges::any_of(bindings_[name], [&](const auto& b) {
    return state_[b].pressed && !state_[b].last_pressed && (state_[b].mods & mods_check) == mods_check;
  });
}

bool InputMgr::released(const std::string& name, const Mods& mods) {
  int mods_check = unwrap(mods);
  return std::ranges::any_of(bindings_[name], [&](const auto& b) {
    return !state_[b].pressed && state_[b].last_pressed && (state_[b].mods & mods_check) == mods_check;
  });
}

// TODO: If the desired mods are released, but the keys aren't, this will keep returning true
bool InputMgr::down(const std::string& name, const Mods& mods, double interval, double delay) {
  int mods_check = unwrap(mods);

  if (interval <= 0.0 && delay <= 0.0) {
    return std::ranges::any_of(bindings_[name], [&](const auto& b) {
      return state_[b].pressed && (state_[b].mods & mods_check) == mods_check;
    });
  }

  if (auto it = repeat_.find(name); it != repeat_.end()) {
    return it->second.pressed;
  }

  for (const auto& action: bindings_[name]) {
    if (state_[action].pressed && (state_[action].mods & mods_check) == mods_check) {
      repeat_[name] = RepeatState{
        .action = action,
        .interval = interval,
        .delay = delay,
        .delay_stage = delay > 0.0
      };

      return delay <= 0.0;
    }
  }

  return false;
}

bool InputMgr::down(const std::string& name, double interval, double delay) {
  return down(name, Mods::none, interval, delay);
}

double InputMgr::mouse_x() const {
  return mouse_state_.x;
}

double InputMgr::mouse_y() const {
  return mouse_state_.y;
}

double InputMgr::mouse_px() const {
  return mouse_state_.px;
}

double InputMgr::mouse_py() const {
  return mouse_state_.py;
}

double InputMgr::mouse_dx() const {
  return mouse_state_.dx;
}

double InputMgr::mouse_dy() const {
  return mouse_state_.dy;
}

bool InputMgr::mouse_moved() const {
  return mouse_state_.moved;
}

bool InputMgr::mouse_got_first_event() const {
  return mouse_state_.got_first_event;
}

double InputMgr::mouse_sx() const {
  return mouse_state_.sx;
}

double InputMgr::mouse_sy() const {
  return mouse_state_.sy;
}

bool InputMgr::mouse_entered() const {
  return mouse_state_.entered == 1;
}

bool InputMgr::mouse_left() const {
  return mouse_state_.entered == -1;
}

void InputMgr::r_update_(const E_Update& p) {
  for (auto& s: state_ | std::views::values) {
    s.last_pressed = s.pressed;
  }

  mouse_state_.px = mouse_state_.x;
  mouse_state_.py = mouse_state_.y;
  mouse_state_.dx = 0.0;
  mouse_state_.dy = 0.0;
  mouse_state_.sx = 0.0;
  mouse_state_.sy = 0.0;
  mouse_state_.moved = false;

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

  for (auto it = repeat_.begin(); it != repeat_.end();) {
    auto& [pressed, action, acc, interval, delay, delay_stage] = it->second;

    if (!state_[action].pressed) {
      it = repeat_.erase(it);
      continue;
    }
    pressed = false;

    if (delay_stage) {
      delay -= p.dt;
      if (delay <= 0) {
        delay_stage = false;
        pressed = true;
      }
    } else {
      acc += p.dt;
      if (acc >= interval) {
        acc -= interval;
        pressed = true;
      }
    }

    ++it;
  }
}

void InputMgr::r_glfw_key_(const E_GlfwKey& p) {
  auto action = glfw_key_to_str_[p.key];

  if (p.action == GLFW_PRESS) {
    action_queue_.emplace(action, true, p.mods, time_nsec<double>());
  } else if (p.action == GLFW_RELEASE) {
    action_queue_.emplace(action, false, p.mods, time_nsec<double>());
  }
}

void InputMgr::r_glfw_character_(const E_GlfwCharacter& p) {}

void InputMgr::r_glfw_cursor_pos_(const E_GlfwCursorPos& p) {
  mouse_state_.dx += p.xpos - mouse_state_.x;
  mouse_state_.dy += p.ypos - mouse_state_.y;
  mouse_state_.x = p.xpos;
  mouse_state_.y = p.ypos;
  mouse_state_.moved = true;

  if (!mouse_state_.got_first_event) {
    mouse_state_.dx = 0;
    mouse_state_.dy = 0;
    mouse_state_.got_first_event = true;
  }
}

void InputMgr::r_glfw_cursor_enter_(const E_GlfwCursorEnter& p) {
  mouse_state_.entered = p.entered ? 1 : -1;
}

void InputMgr::r_glfw_mouse_button_(const E_GlfwMouseButton& p) {
  auto action = glfw_button_to_str_[p.button];

  if (p.action == GLFW_PRESS) {
    action_queue_.emplace(action, true, p.mods, time_nsec<double>());
  } else if (p.action == GLFW_RELEASE) {
    action_queue_.emplace(action, false, p.mods, time_nsec<double>());
  }
}

void InputMgr::r_glfw_scroll_(const E_GlfwScroll& p) {
  mouse_state_.sx += p.xoffset;
  mouse_state_.sy += p.yoffset;
}
} // namespace imp
