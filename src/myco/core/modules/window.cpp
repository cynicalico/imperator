#include "myco/util/log.hpp"

#include "myco/core/modules/window.hpp"
#include "myco/core/engine.hpp"

namespace myco {

void Window::initialize_(const Initialize &e) {
  Module::initialize_(e);

  MYCO_LOG_INFO("Window initialized");
}

} // namespace myco
