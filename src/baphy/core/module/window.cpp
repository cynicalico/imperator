#include "baphy/core/module/window.hpp"

namespace baphy {

void Window::initialize_(const baphy::EInitialize &e) {
  Module::initialize_(e);
}

void Window::shutdown_(const baphy::EShutdown &e) {
  Module::shutdown_(e);
}

} // namespace baphy
