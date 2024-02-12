#include "imp/core/module/application.hpp"

namespace imp {
Application::Application(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
  cursors = module_mgr.lock()->get<CursorMgr>();
  dear = module_mgr.lock()->get<DearImgui>();
  debug_overlay = module_mgr.lock()->get<DebugOverlay>();
  inputs = module_mgr.lock()->get<InputMgr>();
  ctx = module_mgr.lock()->get<GfxContext>();
  timers = module_mgr.lock()->get<TimerMgr>();
  window = module_mgr.lock()->get<Window>();

  IMP_HERMES_SUB(E_StartFrame, module_name, r_start_frame_);
  IMP_HERMES_SUB(E_Draw, module_name, r_draw_);
  IMP_HERMES_SUB(E_EndFrame, module_name, r_end_frame_);
  IMP_HERMES_SUB(E_Update, module_name, r_update_, InputMgr, TimerMgr, Window);
}

void Application::r_start_frame_(const E_StartFrame& p) {
  dear->new_frame();
}

void Application::r_draw_(const E_Draw& p) {
  draw();
}

void Application::r_end_frame_(const E_EndFrame& p) {
  dear->render();
}

void Application::r_update_(const E_Update& p) {
  update(p.dt);
}
} // namespace imp
