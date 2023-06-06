#ifndef BAPHY_CORE_MODULE_DEAR_IMGUI_HPP
#define BAPHY_CORE_MODULE_DEAR_IMGUI_HPP

#include "baphy/core/module/gfx_context.hpp"
#include "baphy/core/module/module.hpp"
#include "baphy/core/module/window.hpp"
#include "imgui.h"
//#include "implot.h"
#include <memory>

namespace baphy {

class DearImgui : public Module<DearImgui> {
public:
  DearImgui() : Module<DearImgui>({EPI<GfxContext>::name, EPI<Window>::name}) {}

  ~DearImgui() override = default;

  void new_frame();

  void render();

private:
  ImGuiContext *ctx_{nullptr};
  ImGuiIO *io_{nullptr};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::DearImgui);

#endif//BAPHY_CORE_MODULE_DEAR_IMGUI_HPP
