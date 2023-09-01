#include "baphy/gfx/module/gui_mgr.hpp"

namespace baphy {

void GuiMgr::e_initialize_(const EInitialize &e) {
  input = module_mgr->get<InputMgr>();
  primitives = module_mgr->get<PrimitiveBatcher>();

  EventBus::sub<EUpdate>(module_name, {EPI<InputMgr>::name}, [&](const auto &e) { e_update_(e); });

  Module::e_initialize_(e);
}

void GuiMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void GuiMgr::e_update_(const EUpdate &e) {
  bool prev_layout_is_active{false};
  std::size_t active_layout_idx = layouts_.size();

  for (const auto &[i, l]: enumerate(layouts_ | std::views::reverse)) {
    if (!l->parent) {
      l->update(e.dt, 0.0, 0.0, !prev_layout_is_active);

      if (!prev_layout_is_active && l->active) {
        prev_layout_is_active = true;
        active_layout_idx = layouts_.size() - 1 - i;
      }
    }
  }

  if (active_layout_idx != layouts_.size())
    range_move_to_back(layouts_, active_layout_idx);
}

} // namespace baphy
