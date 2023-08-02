#include "baphy/gfx/module/gui_mgr.hpp"

namespace baphy {

void GuiMgr::e_initialize_(const EInitialize &e) {
  Module::e_initialize_(e);
}

void GuiMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

} // namespace baphy
