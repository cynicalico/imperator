#ifndef IMPERATOR_CORE_MODULE_INPUT_MGR_HPP
#define IMPERATOR_CORE_MODULE_INPUT_MGR_HPP

#include "imperator/core/module_mgr.hpp"
#include "imperator/core/module/window.hpp"
#include <mutex>
#include <unordered_map>
#include <vector>
#include <queue>

namespace imp {
class InputMgr : public Module<InputMgr> {
public:
  InputMgr() : Module({
    EPI<Window>::name
  }) {}

  void bind(const std::string& name, const std::string& action);

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
    double time{0};
  };
  std::unordered_map<std::string, ActionState> state_{};

  struct PendingAction {
    std::string action{};
    bool pressed{false};
    int mods{};
    double time{0};
  };
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

IMPERATOR_PRAISE_HERMES(imp::InputMgr);

#endif//IMPERATOR_CORE_MODULE_INPUT_MGR_HPP
