#ifndef BAPHY_GFX_MODULE_DEAR_IMGUI_HPP
#define BAPHY_GFX_MODULE_DEAR_IMGUI_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "imgui.h"
#include "implot.h"

namespace baphy {
class DearImgui : public Module<DearImgui> {
public:
  DearImgui() : Module({
    EPI<GfxContext>::name,
    EPI<Window>::name,
  }) {}

  void new_frame();

  void render();

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  ImGuiContext* ctx_{nullptr};
  ImGuiIO* io_{nullptr};

  ImPlotContext* implot_ctx_{nullptr};
};
} // namespace baphy

BAPHY_PRAISE_HERMES(baphy::DearImgui);

#endif//BAPHY_GFX_MODULE_DEAR_IMGUI_HPP
