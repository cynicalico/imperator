#ifndef BAPHY_UTIL_MODULE_TIMER_MGR_HPP
#define BAPHY_UTIL_MODULE_TIMER_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/util/rnd.hpp"
#include <concepts>
#include <string>
#include <unordered_map>

template<typename T>
concept AfterFunc = requires(T f) {
  { f() };
};

template<typename T>
concept EveryFunc = requires(T f) {
  { f() };
};

template<typename T>
concept UntilFunc = requires(T f) {
  { f() } -> std::convertible_to<bool>;
};

namespace baphy {

class TimerMgr : public Module<TimerMgr> {
public:
  TimerMgr() : Module<TimerMgr>() {}

  ~TimerMgr() override = default;

  template<typename AfterFunc>
  std::string after(const std::string &tag, double delay, AfterFunc &&f);

  template<typename AfterFunc>
  std::string after(double delay, AfterFunc &&f);

private:
  struct TimerI {
    double interval{0.0};
    double acc{0.0};
    bool should_fire{false};
    bool expired{false};
    bool paused{false};

    TimerI(double interval) : interval(interval) {}
    virtual ~TimerI() = default;

    virtual void update(double dt) = 0;
    virtual void fire() = 0;
  };

  template<typename AfterFunc>
  struct AfterTimer : public TimerI {
    AfterFunc f;

    AfterTimer(double interval, AfterFunc &&f)
        : TimerI(interval), f(std::forward<AfterFunc>(f)) {}

    void update(double dt) override;
    void fire() override;
  };

  std::unordered_map<std::string, std::unique_ptr<TimerI>> timers_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
};

template<typename AfterFunc>
std::string TimerMgr::after(const std::string &tag, double delay, AfterFunc &&f) {
  timers_[tag] = std::make_unique<AfterTimer<AfterFunc>>(delay, std::forward<AfterFunc>(f));
  return tag;
}

template<typename AfterFunc>
std::string TimerMgr::after(double delay, AfterFunc &&f) {
  return after(base58(11), delay, std::forward<AfterFunc>(f));
}

template<typename T>
void TimerMgr::AfterTimer<T>::update(double dt) {
  acc += dt;

  if (acc >= interval)
    should_fire = true;
}

template<typename T>
void TimerMgr::AfterTimer<T>::fire() {
  f();

  expired = true;
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::TimerMgr);

#endif//BAPHY_UTIL_MODULE_TIMER_MGR_HPP
