#include "imp/core/module/application.hpp"

namespace imp {
void Application::r_initialize_(const E_Initialize& e) {
  cursors = module_mgr->get<CursorMgr>();
  dear = module_mgr->get<DearImgui>();
  debug_overlay = module_mgr->get<DebugOverlay>();
  inputs = module_mgr->get<InputMgr>();
  gfx = module_mgr->get<GfxContext>();
  timers = module_mgr->get<TimerMgr>();
  window = module_mgr->get<Window>();

  IMP_HERMES_SUB(E_StartFrame, module_name, r_start_frame_);
  IMP_HERMES_SUB(E_Draw, module_name, r_draw_);
  IMP_HERMES_SUB(E_EndFrame, module_name, r_end_frame_);
  IMP_HERMES_SUB(E_Update, module_name, r_update_, InputMgr, TimerMgr, Window);

  initialize();

  Module::r_initialize_(e);
}

void Application::r_shutdown_(const E_Shutdown& e) {
  Module::r_shutdown_(e);
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
