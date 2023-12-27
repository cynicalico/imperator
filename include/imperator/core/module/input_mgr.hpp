#ifndef IMPERATOR_CORE_MODULE_INPUT_MGR_HPP
#define IMPERATOR_CORE_MODULE_INPUT_MGR_HPP

#include "imperator/core/module_mgr.hpp"
#include "imperator/core/module/window.hpp"
#include <mutex>
#include <unordered_map>
#include <vector>
#include <queue>

namespace imp {
enum class Mods {
  none = 0,
  shift = 1 << 0,
  ctrl = 1 << 1,
  alt = 1 << 2,
  super = 1 << 3,
  caps = 1 << 4,
  numlock = 1 << 5
};

class InputMgr : public Module<InputMgr> {
public:
  InputMgr() : Module({
    EPI<Window>::name
  }) {}

  void bind(const std::string& name, const std::string& action);
  std::vector<int> get_glfw_actions(const std::string& name);

  bool pressed(const std::string& name, const Mods& mods = Mods::none);

  bool released(const std::string& name, const Mods& mods = Mods::none);

  bool down(const std::string& name, const Mods& mods, double interval = 0.0, double delay = 0.0);
  bool down(const std::string& name, double interval = 0.0, double delay = 0.0);

  double mouse_x() const;
  double mouse_y() const;

  double mouse_px() const;
  double mouse_py() const;

  double mouse_dx() const;
  double mouse_dy() const;

  bool mouse_moved() const;

  bool mouse_got_first_event() const;

  double mouse_sx() const;
  double mouse_sy() const;

  bool mouse_entered() const;
  bool mouse_left() const;

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  static std::once_flag initialize_glfw_action_maps_;

  static std::vector<int> all_glfw_keys_;
  static std::vector<std::string> all_str_keys_;
  static std::unordered_map<int, std::string> glfw_key_to_str_;
  static std::unordered_map<std::string, int> str_to_glfw_key_;

  static std::vector<int> all_glfw_buttons_;
  static std::vector<std::string> all_str_buttons_;
  static std::unordered_map<int, std::string> glfw_button_to_str_;
  static std::unordered_map<std::string, int> str_to_glfw_button_;

  std::unordered_map<std::string, std::vector<std::string>> bindings_{};

  struct ActionState {
    bool pressed{false};
    bool last_pressed{false};
    int mods{};
    double press_time{0};
    double release_time{0};
  };

  struct RepeatState {
    bool pressed{false};
    std::string action;
    double acc{0};
    double interval{0};
    double delay{0};
    bool delay_stage{false};
  };

  struct PendingAction {
    std::string action{};
    bool pressed{false};
    int mods{};
    double time{0};
  };

  struct {
    double x{0}, y{0};
    double px{0}, py{0};
    double dx{0}, dy{0};
    bool moved{false};
    bool got_first_event{false};

    double sx{0}, sy{0};

    int entered{0};
  } mouse_state_{};
  std::unordered_map<std::string, ActionState> state_{};
  std::unordered_map<std::string, RepeatState> repeat_{};
  std::queue<PendingAction> action_queue_{};

  void r_update_(const E_Update& p);
  void r_glfw_key_(const E_GlfwKey& p);
  void r_glfw_character_(const E_GlfwCharacter& p);
  void r_glfw_cursor_pos_(const E_GlfwCursorPos& p);
  void r_glfw_cursor_enter_(const E_GlfwCursorEnter& p);
  void r_glfw_mouse_button_(const E_GlfwMouseButton& p);
  void r_glfw_scroll_(const E_GlfwScroll& p);
};
} // namespace imp

ENUM_ENABLE_BITOPS(imp::Mods);

IMPERATOR_PRAISE_HERMES(imp::InputMgr);

#endif//IMPERATOR_CORE_MODULE_INPUT_MGR_HPP
