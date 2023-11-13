#ifndef BAPHY_CORE_MODULE_APPLICATION_HPP
#define BAPHY_CORE_MODULE_APPLICATION_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/dear_imgui.hpp"
#include "baphy//gfx/module/gfx_context.hpp"

namespace baphy {
class Application : public Module<Application> {
public:
  std::shared_ptr<DearImgui> dear{nullptr};
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<Window> window{nullptr};

  Application() : Module({
    EPI<DearImgui>::name,
    EPI<GfxContext>::name,
    EPI<Window>::name,
  }) {}

  virtual void initialize() {}
  virtual void update(double dt) {}
  virtual void draw() {}

protected:
  void r_initialize_(const E_Initialize& e) override;
  void r_shutdown_(const E_Shutdown& e) override;

private:
  void r_start_frame_(const E_StartFrame& p);
  void r_draw_(const E_Draw& p);
  void r_end_frame_(const E_EndFrame& p);
  void r_update_(const E_Update& p);
};
} // namespace baphy

BAPHY_PRAISE_HERMES(baphy::Application);

#endif//BAPHY_CORE_MODULE_APPLICATION_HPP
