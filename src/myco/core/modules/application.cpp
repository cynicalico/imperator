#include "myco/util/log.hpp"

#include "myco/core/modules/application.hpp"
#include "myco/core/engine.hpp"

namespace myco {

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::initialize_(const Initialize &e) {
  Module<Application>::initialize_(e);

  Scheduler::sub<Update>(name, [&](const auto &e){ update(e.dt); });
  Scheduler::sub<Draw>(name, [&](const auto &){ draw(); });

  window = engine->get_module<Window>();

  initialize();
}

} // namespace myco
