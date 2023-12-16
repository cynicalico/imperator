#include "imperator/core/module/application.hpp"

namespace imp {
void Application::r_initialize_(const E_Initialize& e) {
  dear = module_mgr->get<DearImgui>();
  input = module_mgr->get<InputMgr>();
  gfx = module_mgr->get<GfxContext>();
  window = module_mgr->get<Window>();

  Hermes::sub<E_StartFrame>(module_name, [&](const auto& p) { r_start_frame_(p); });
  Hermes::sub<E_Draw>(module_name, [&](const auto& p) { r_draw_(p); });
  Hermes::sub<E_EndFrame>(module_name, [&](const auto& p) { r_end_frame_(p); });
  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });

  initialize();

  Module::r_initialize_(e);
}

void Application::r_shutdown_(const E_Shutdown& e) {
  Module::r_shutdown_(e);
}

void Application::r_start_frame_(const E_StartFrame& p) {}

void Application::r_draw_(const E_Draw& p) {
  draw();
}

void Application::r_end_frame_(const E_EndFrame& p) {}

void Application::r_update_(const E_Update& p) {
  update(p.dt);
}
} // namespace imp
