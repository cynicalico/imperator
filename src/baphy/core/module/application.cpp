#include "baphy/core/module/application.hpp"

namespace baphy {

void Application::initialize() {}

void Application::update(double dt) {}

void Application::draw() {}

void Application::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<EStartApplication>(module_name, [&](const auto &e) { e_start_application_(e); });
  EventBus::sub<EUpdate>(module_name, {EPI<Window>::name, EPI<InputMgr>::name}, [&](const auto &e) { e_update_(e); });
  EventBus::sub<EStartFrame>(module_name, {EPI<Window>::name}, [&](const auto &e) { e_start_frame_(e); });
  EventBus::sub<EDraw>(module_name, [&](const auto &e) { e_draw_(e); });
  EventBus::sub<EEndFrame>(module_name, [&](const auto &e) { e_end_frame_(e); });

  dear = module_mgr->get<DearImgui>();
  input = module_mgr->get<InputMgr>();
  window = module_mgr->get<Window>();
  gfx = module_mgr->get<GfxContext>();

  Module::e_initialize_(e);
}

void Application::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Application::e_start_application_(const baphy::EStartApplication &e) {
  initialize();
}

void Application::e_update_(const EUpdate &e) {
  update(e.dt);
}

void Application::e_start_frame_(const EStartFrame &e) {}

void Application::e_draw_(const EDraw &e) {
  draw();
}

void Application::e_end_frame_(const EEndFrame &e) {}

} // namespace baphy
