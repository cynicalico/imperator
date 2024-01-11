#ifndef IMP_GFX_MODULE_DEAR_IMGUI_HPP
#define IMP_GFX_MODULE_DEAR_IMGUI_HPP

#include "imp/core/module/window.hpp"
#include "imp/core/module_mgr.hpp"
#include "imp/gfx/module/gfx_context.hpp"
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

IMP_PRAISE_HERMES(imp::DearImgui);

#endif//IMP_GFX_MODULE_DEAR_IMGUI_HPP
