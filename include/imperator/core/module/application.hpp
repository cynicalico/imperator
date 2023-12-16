#ifndef IMPERATOR_CORE_MODULE_APPLICATION_HPP
#define IMPERATOR_CORE_MODULE_APPLICATION_HPP

#include "imperator/core/module/window.hpp"
#include "imperator/core/module/input_mgr.hpp"
#include "imperator/core/module_mgr.hpp"
#include "imperator/gfx/module/dear_imgui.hpp"
#include "imperator//gfx/module/gfx_context.hpp"

namespace imp {
class Application : public Module<Application> {
public:
  std::shared_ptr<DearImgui> dear{nullptr};
  std::shared_ptr<InputMgr> input{nullptr};
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<Window> window{nullptr};

  Application() : Module({
    EPI<DearImgui>::name,
    EPI<InputMgr>::name,
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
} // namespace imp

IMPERATOR_PRAISE_HERMES(imp::Application);

#endif//IMPERATOR_CORE_MODULE_APPLICATION_HPP
