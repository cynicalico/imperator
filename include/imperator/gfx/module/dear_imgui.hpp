#ifndef IMPERATOR_GFX_MODULE_DEAR_IMGUI_HPP
#define IMPERATOR_GFX_MODULE_DEAR_IMGUI_HPP

#include "imperator/core/module/window.hpp"
#include "imperator/core/module_mgr.hpp"
#include "imperator/gfx/module/gfx_context.hpp"
#include "imgui.h"
#include "implot.h"
#include "misc/cpp/imgui_stdlib.h"

namespace imp {
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

  void setup_style_();
};
} // namespace imp

IMPERATOR_PRAISE_HERMES(imp::DearImgui);

#endif//IMPERATOR_GFX_MODULE_DEAR_IMGUI_HPP
