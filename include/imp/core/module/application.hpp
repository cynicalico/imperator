#ifndef IMP_CORE_MODULE_APPLICATION_HPP
#define IMP_CORE_MODULE_APPLICATION_HPP

#include "imp/core/module/cursor_mgr.hpp"
#include "imp/core/module/input_mgr.hpp"
#include "imp/core/module/window.hpp"
#include "imp/core/module_mgr.hpp"
#include "imp/gfx/module/dear_imgui.hpp"
#include "imp/gfx/module/gfx_context.hpp"
#include "imp/util/module/debug_overlay.hpp"
#include "imp/util/module/timer_mgr.hpp"

namespace imp {
class Application : public Module<Application> {
public:
  std::shared_ptr<CursorMgr> cursors{nullptr};
  std::shared_ptr<DearImgui> dear{nullptr};
  std::shared_ptr<DebugOverlay> debug_overlay{nullptr};
  std::shared_ptr<InputMgr> inputs{nullptr};
  std::shared_ptr<GfxContext> ctx{nullptr};
  std::shared_ptr<TimerMgr> timers{nullptr};
  std::shared_ptr<Window> window{nullptr};

  Application() : Module({
    EPI<CursorMgr>::name,
    EPI<DearImgui>::name,
    EPI<DebugOverlay>::name,
    EPI<InputMgr>::name,
    EPI<GfxContext>::name,
    EPI<TimerMgr>::name,
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

IMP_PRAISE_HERMES(imp::Application);

#endif//IMP_CORE_MODULE_APPLICATION_HPP
