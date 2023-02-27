#include "myco/util/log.hpp"

#include "myco/core/modules/application.hpp"
#include "myco/core/engine.hpp"

namespace myco {

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::initialize_(const Initialize &e) {
  Module<Application>::initialize_(e);

  window = engine->get_module<Window>();

  initialize();
}

} // namespace myco
