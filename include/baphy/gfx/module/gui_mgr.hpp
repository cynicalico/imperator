#ifndef BAPHY_GFX_MODULE_GUI_MGR_HPP
#define BAPHY_GFX_MODULE_GUI_MGR_HPP

#include "baphy/core/module/input_mgr.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/surface_mgr.hpp"

namespace baphy {

class GuiMgr : public Module<GuiMgr> {
public:
  GuiMgr() : Module<GuiMgr>({EPI<InputMgr>::name, EPI<SurfaceMgr>::name}) {}

private:
  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::GuiMgr);

#endif//BAPHY_GFX_MODULE_GUI_MGR_HPP
