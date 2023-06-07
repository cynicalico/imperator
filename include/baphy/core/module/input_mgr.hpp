#ifndef BAPHY_CORE_MODULE_INPUT_MGR_HPP
#define BAPHY_CORE_MODULE_INPUT_MGR_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include <unordered_map>
#include <unordered_set>

namespace baphy {

class InputMgr : public Module<InputMgr> {
public:
  InputMgr() : Module<InputMgr>({EPI<Window>::name}) {}

  ~InputMgr() override = default;

  double mouse_x();
  double mouse_y();
  double mouse_px();
  double mouse_py();
  double mouse_dx();
  double mouse_dy();

  bool pressed(const std::string &binding);
  bool released(const std::string &binding);
  bool down(const std::string &binding, double interval = 0.0, double delay = 0.0);

private:
  struct ActionInfo {
    bool pressed{false};
    bool last_pressed{false};
    double time{0.0};
  };
  std::unordered_map<std::string, ActionInfo> state_{};

  std::string glfw_key_to_str_(int key);
  std::string glfw_button_to_str_(int button);
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
