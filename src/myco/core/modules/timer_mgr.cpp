#include "myco/core/modules/timer_mgr.hpp"

namespace myco {

void TimerMgr::initialize_(const Initialize &e) {
  Module<TimerMgr>::initialize_(e);

  Scheduler::sub<Update>(name, [&](const auto &e){ update_(e.dt); });
}

void TimerMgr::update_(double dt) {
  for (auto it = timers_.begin(); it != timers_.end(); ) {
    if (it->second->paused) {
      it++;
      continue;
    }

    if (it->second->update(dt))
      it->second->fire();

    if (it->second->expired)
      it = timers_.erase(it);

    else
      it++;
  }
}

void TimerMgr::pause(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = true;
}

void TimerMgr::resume(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = false;
}

void TimerMgr::toggle(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = !it->second->paused;
}

void TimerMgr::cancel(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    timers_.erase(it);
}

void TimerMgr::cancel_all() {
  timers_.clear();
}

} // namespace myco
