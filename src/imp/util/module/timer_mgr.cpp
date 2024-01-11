#include "imp/util/module/timer_mgr.hpp"

namespace imp {
void TimerMgr::cancel(const std::string& tag) {
  if (auto it = timers_.find(tag); it != timers_.end())
    timers_.erase(it);
}

void TimerMgr::cancel_all() {
  timers_.clear();
}

void TimerMgr::pause(const std::string& tag) {
  if (auto it = timers_.find(tag); it != timers_.end())
    it->second->paused = true;
}

void TimerMgr::resume(const std::string& tag) {
  if (auto it = timers_.find(tag); it != timers_.end())
    it->second->paused = false;
}

void TimerMgr::toggle(const std::string& tag) {
  if (auto it = timers_.find(tag); it != timers_.end())
    it->second->paused = !it->second->paused;
}

bool TimerMgr::is_paused(const std::string& tag) {
  if (auto it = timers_.find(tag); it != timers_.end())
    return it->second->paused;
  return false;
}

void TimerMgr::r_initialize_(const E_Initialize& p) {
  IMP_HERMES_SUB(E_Update, module_name, r_update_);

  Module::r_initialize_(p);
}

void TimerMgr::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}

void TimerMgr::r_update_(const E_Update& p) {
  for (auto it = timers_.begin(); it != timers_.end();) {
    if (const auto& t = it->second; !t->paused) {
      t->update(p.dt);
      if (t->should_fire) {
        t->fire();
      }

      if (t->expired) {
        it = timers_.erase(it);
        continue;
      }
    }

    ++it;
  }
}
} // namespace imp
