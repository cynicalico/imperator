#include "baphy/core/module/application.hpp"

namespace baphy {

void Application::initialize() {}

void Application::update(double dt) {}

void Application::draw() {}

void Application::initialize_(const baphy::EInitialize &e) {
  Module::initialize_(e);

  window = module_mgr->get<Window>();
}

void Application::shutdown_(const baphy::EShutdown &e) {
  Module::shutdown_(e);
}

} // namespace baphy
