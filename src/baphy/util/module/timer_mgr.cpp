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
  for (auto it = timers_.begin(); it != timers_.end(); ) {
    auto &t = it->second;

    if (!t->paused) {
      t->update(e.dt);
      if (t->should_fire) {
        t->fire();
      }

      if (t->expired) {
        it = timers_.erase(it);
        continue;
      }
    }

    it++;
  }
}

} // namespace baphy
