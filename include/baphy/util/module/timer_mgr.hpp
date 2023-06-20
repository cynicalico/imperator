#ifndef BAPHY_UTIL_MODULE_TIMER_MGR_HPP
#define BAPHY_UTIL_MODULE_TIMER_MGR_HPP

#include "baphy/core/module_mgr.hpp"

namespace baphy {

class TimerMgr : public Module<TimerMgr> {
public:
  TimerMgr() : Module<TimerMgr>() {}

  ~TimerMgr() override = default;

private:
  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::TimerMgr);

#endif//BAPHY_UTIL_MODULE_TIMER_MGR_HPP
