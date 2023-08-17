#include "baphy/gfx/module/font_mgr.hpp"

namespace baphy {

void FontMgr::e_initialize_(const EInitialize &e) {
  Module::e_initialize_(e);
}

void FontMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

} // namespace baphy
