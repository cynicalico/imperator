#ifndef BAPHY_CORE_MODULE_CURSOR_MGR_HPP
#define BAPHY_CORE_MODULE_CURSOR_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/core/module/window.hpp"

namespace baphy {

class CursorMgr : public Module<CursorMgr> {
public:
  CursorMgr() : Module<CursorMgr>({EPI<Window>::name}) {}
  ~CursorMgr() override = default;

  bool imgui_hovered();
  ImGuiMouseCursor imgui_cursor();

private:
  bool received_hovered_{false};
  ImGuiMouseCursor imgui_cursor_{ImGuiMouseCursor_None};

  bool received_hovered_last_{false};
  ImGuiMouseCursor imgui_cursor_last_{ImGuiMouseCursor_None};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_end_frame_(const EEndFrame &e);
  void e_imgui_window_hovered_(const EImguiWindowHovered &e);
  void e_imgui_cursor_(const EImguiCursor &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::CursorMgr);

#endif//BAPHY_CORE_MODULE_CURSOR_MGR_HPP
