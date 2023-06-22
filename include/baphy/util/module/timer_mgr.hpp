#ifndef BAPHY_UTIL_MODULE_TIMER_MGR_HPP
#define BAPHY_UTIL_MODULE_TIMER_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/util/rnd.hpp"
#include <concepts>
#include <string>
#include <unordered_map>
#include <vector>

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

  template<EveryFunc T>
  std::string every(const std::string &tag, const std::vector<double> &delay_opts, std::int64_t count, T &&f);

  template<EveryFunc T>
  std::string every(const std::string &tag, double delay, std::int64_t count, T &&f);

  template<EveryFunc T>
  std::string every(const std::string &tag, const std::vector<double> &delay_opts, T &&f);

  template<EveryFunc T>
  std::string every(const std::string &tag, double delay, T &&f);

  template<EveryFunc T>
  std::string every(const std::vector<double> &delay_opts, std::int64_t count, T &&f);

  template<EveryFunc T>
  std::string every(double delay, std::int64_t count, T &&f);

  template<EveryFunc T>
  std::string every(const std::vector<double> &delay_opts, T &&f);

  template<EveryFunc T>
  std::string every(double delay, T &&f);

private:
  struct TimerI {
    double delay{0.0};
    double acc{0.0};
    bool should_fire{false};
    bool expired{false};
    bool paused{false};

    TimerI(double delay) : delay(delay) {}
    virtual ~TimerI() = default;

    virtual void update(double dt) = 0;
    virtual void fire() = 0;
  };

  template<typename AfterFunc>
  struct AfterTimer : public TimerI {
    AfterFunc f;

    AfterTimer(double delay, AfterFunc &&f)
        : TimerI(delay), f(std::forward<AfterFunc>(f)) {}

    void update(double dt) override;
    void fire() override;
  };

  template<typename EveryFunc>
  struct EveryTimer : public TimerI {
    EveryFunc f;
    std::vector<double> delay_opts;
    std::int64_t count, count_acc{0};

    EveryTimer(const std::vector<double> &delay_opts, std::int64_t count, EveryFunc &&f)
        : TimerI(choose(delay_opts)), delay_opts(delay_opts), count(count), f(std::forward<EveryFunc>(f)) {}

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

template<EveryFunc T>
std::string TimerMgr::every(const std::string &tag, const std::vector<double> &delay_opts, std::int64_t count, T &&f) {
  timers_[tag] = std::make_unique<EveryTimer<T>>(delay_opts, count, std::forward<T>(f));
  return tag;
}

template<EveryFunc T>
std::string TimerMgr::every(const std::string &tag, double delay, std::int64_t count, T &&f) {
  return every(tag, std::vector{delay}, count, std::forward<T>(f));
}

template<EveryFunc T>
std::string TimerMgr::every(const std::string &tag, const std::vector<double> &delay_opts, T &&f) {
  return every(tag, delay_opts, -1, std::forward<T>(f));
}

template<EveryFunc T>
std::string TimerMgr::every(const std::string &tag, double delay, T &&f) {
  return every(tag, std::vector{delay}, -1, std::forward<T>(f));
}

template<EveryFunc T>
std::string TimerMgr::every(const std::vector<double> &delay_opts, std::int64_t count, T &&f) {
  return every(base58(11), delay_opts, count, std::forward<T>(f));
}

template<EveryFunc T>
std::string TimerMgr::every(double delay, std::int64_t count, T &&f) {
  return every(base58(11), std::vector{delay}, count, std::forward<T>(f));
}

template<EveryFunc T>
std::string TimerMgr::every(const std::vector<double> &delay_opts, T &&f) {
  return every(base58(11), delay_opts, -1, std::forward<T>(f));
}

template<EveryFunc T>
std::string TimerMgr::every(double delay, T &&f) {
  return every(base58(11), std::vector{delay}, -1, std::forward<T>(f));
}

template<typename T>
void TimerMgr::AfterTimer<T>::update(double dt) {
  acc += dt;

  if (acc >= delay)
    should_fire = true;
}

template<typename T>
void TimerMgr::AfterTimer<T>::fire() {
  f();

  expired = true;
}

template<typename T>
void TimerMgr::EveryTimer<T>::update(double dt) {
  acc += dt;

  if (acc >= delay)
    should_fire = true;
}

template<typename T>
void TimerMgr::EveryTimer<T>::fire() {
  f();
  should_fire = false;
  acc -= delay;
  delay = choose(delay_opts);

  count_acc++;
  if (count_acc == count)
    expired = true;
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::TimerMgr);

#endif//BAPHY_UTIL_MODULE_TIMER_MGR_HPP
