#include "baphy/core/module/application.hpp"

namespace baphy {

void Application::r_initialize_(const E_Initialize& e) {
  Module::r_initialize_(e);
}

void Application::r_shutdown_(const E_Shutdown& e) {
  Module::r_shutdown_(e);
}

} // namespace baphy
