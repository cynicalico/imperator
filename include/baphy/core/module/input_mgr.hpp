#ifndef BAPHY_CORE_MODULE_INPUT_MGR_HPP
#define BAPHY_CORE_MODULE_INPUT_MGR_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace baphy {

class InputMgr : public Module<InputMgr> {
public:
  std::shared_ptr<Window> window{nullptr};

  InputMgr() : Module<InputMgr>({EPI<Window>::name}) {}

  ~InputMgr() override = default;

  void bind(const std::string &name, const std::string &action);
  void remove_binding(const std::string &name, const std::string &action);

  double mouse_x() const;
  double mouse_y() const;
  double mouse_px() const;
  double mouse_py() const;
  double mouse_dx() const;
  double mouse_dy() const;
  double mouse_sx() const;
  double mouse_sy() const;
  bool mouse_moved() const;
  bool mouse_entered() const;
  bool mouse_got_first_event() const;

  void show_cursor();
  void hide_cursor();
  void lock_cursor();

  bool pressed(const std::string &binding);
  bool released(const std::string &binding);
  bool down(const std::string &binding, double interval = 0.0, double delay = 0.0);

  void force_update_state_(const std::string &binding);

private:
  struct {
    double x{0.0};
    double y{0.0};
    double px{0.0};
    double py{0.0};
    double dx{0.0};
    double dy{0.0};
    double sx{0.0};
    double sy{0.0};
    bool moved{false};
    bool entered{false};
    bool got_first_event{false};
  } mouse_info_{};

  struct ActionInfo {
    bool pressed{false};
    bool last_pressed{false};
    std::uint64_t time{0};
  };
  struct RepeatInfo {
    std::string action{};
    double interval{0};
    double delay{0};
    bool delay_stage{false};
    double acc{0};
    bool pressed{false};
  };
  std::unordered_map<std::string, ActionInfo> state_{};
  std::unordered_map<std::string, RepeatInfo> repeat_{};
  std::unordered_map<std::string, std::vector<std::string>> bindings_{};

  struct ActionQueueItem {
    std::string action;
    bool pressed{false};
    std::uint64_t time{0};
  };
  std::queue<ActionQueueItem> action_queue_{};

  std::string glfw_key_to_str_(int key);
  std::string glfw_button_to_str_(int button);
  int str_to_glfw_key_(const std::string &str);
  int str_to_glfw_button_(const std::string &str);
  const std::unordered_set<std::string> &all_glfw_actions_();

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);

  void e_glfw_key_(const EGlfwKey &e);
  void e_glfw_cursor_pos_(const EGlfwCursorPos &e);
  void e_glfw_cursor_enter_(const EGlfwCursorEnter &e);
  void e_glfw_mouse_button_(const EGlfwMouseButton &e);
  void e_glfw_scroll_(const EGlfwScroll &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::InputMgr);

#endif//BAPHY_CORE_MODULE_INPUT_MGR_HPP
