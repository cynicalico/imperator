#include "baphy/util/module/timer_mgr.hpp"

namespace baphy {

void TimerMgr::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });

  Module::e_initialize_(e);
}

void TimerMgr::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void TimerMgr::e_update_(const EUpdate &e) {

}

} // namespace baphy
