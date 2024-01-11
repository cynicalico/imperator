#ifndef IMPERATOR_UTIL_MODULE_TIMER_MGR_HPP
#define IMPERATOR_UTIL_MODULE_TIMER_MGR_HPP

#include "imperator/core/module_mgr.hpp"
#include "imperator/util/rnd.hpp"
#include <concepts>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace imp {
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

struct no_op {
  template<class... Args>
  constexpr void operator()(Args&&...) const {}
};

class TimerMgr : public Module<TimerMgr> {
public:
  TimerMgr(): Module() {}

  void cancel(const std::string& tag);
  void cancel_all();

  void pause(const std::string& tag);
  void resume(const std::string& tag);
  void toggle(const std::string& tag);

  bool is_paused(const std::string& tag);

  template<AfterFunc T>
  std::string after(const std::string& tag, double delay, T&& f);

  template<AfterFunc T>
  std::string after(double delay, T&& f);

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(const std::string& tag, const std::vector<double>& delay_opts, std::int64_t count, T&& f,
                    T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(const std::string& tag, double delay, std::int64_t count, T&& f, T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(const std::string& tag, const std::vector<double>& delay_opts, T&& f, T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(const std::string& tag, double delay, T&& f, T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(const std::vector<double>& delay_opts, std::int64_t count, T&& f, T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(double delay, std::int64_t count, T&& f, T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(const std::vector<double>& delay_opts, T&& f, T2&& after_f = T2());

  template<EveryFunc T, AfterFunc T2 = no_op>
  std::string every(double delay, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(const std::string& tag, const std::vector<double>& delay_opts, std::int64_t count, T&& f,
                    T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(const std::string& tag, double delay, std::int64_t count, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(const std::string& tag, const std::vector<double>& delay_opts, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(const std::string& tag, double delay, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(const std::vector<double>& delay_opts, std::int64_t count, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(double delay, std::int64_t count, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(const std::vector<double>& delay_opts, T&& f, T2&& after_f = T2());

  template<UntilFunc T, AfterFunc T2 = no_op>
  std::string until(double delay, T&& f, T2&& after_f = T2());

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

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

    AfterTimer(double delay, AfterFunc&& f)
      : TimerI(delay), f(std::forward<AfterFunc>(f)) {}

    void update(double dt) override;
    void fire() override;
  };

  template<typename EveryFunc, typename AfterFunc>
  struct EveryTimer : public TimerI {
    EveryFunc f;
    AfterFunc f2;
    std::vector<double> delay_opts;
    std::int64_t count, count_acc{0};

    EveryTimer(const std::vector<double>& delay_opts, std::int64_t count, EveryFunc&& f, AfterFunc&& f2)
      : TimerI(rnd::choose(delay_opts)), delay_opts(delay_opts), count(count), f(std::forward<EveryFunc>(f)),
        f2(std::forward<AfterFunc>(f2)) {}

    void update(double dt) override;
    void fire() override;
  };

  template<typename UntilFunc, typename AfterFunc>
  struct UntilTimer : public TimerI {
    UntilFunc f;
    AfterFunc f2;
    std::vector<double> delay_opts;
    std::int64_t count, count_acc{0};

    UntilTimer(const std::vector<double>& delay_opts, std::int64_t count, UntilFunc&& f, AfterFunc&& f2)
      : TimerI(rnd::choose(delay_opts)), delay_opts(delay_opts), count(count), f(std::forward<UntilFunc>(f)),
        f2(std::forward<AfterFunc>(f2)) {}

    void update(double dt) override;
    void fire() override;
  };

  std::unordered_map<std::string, std::unique_ptr<TimerI>> timers_{};

  void r_update_(const E_Update& p);
};

template<AfterFunc T>
std::string TimerMgr::after(const std::string& tag, double delay, T&& f) {
  timers_[tag] = std::make_unique<AfterTimer<T>>(delay, std::forward<T>(f));
  return tag;
}

template<AfterFunc T>
std::string TimerMgr::after(double delay, T&& f) {
  return after(rnd::base58(11), delay, std::forward<T>(f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(const std::string& tag, const std::vector<double>& delay_opts, std::int64_t count, T&& f,
                            T2&& after_f) {
  timers_[tag] = std::make_unique<EveryTimer<T, T2>>(delay_opts, count, std::forward<T>(f), std::forward<T2>(after_f));
  return tag;
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(const std::string& tag, double delay, std::int64_t count, T&& f, T2&& after_f) {
  return every(tag, std::vector{delay}, count, std::forward<T>(f), std::forward<T2>(after_f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(const std::string& tag, const std::vector<double>& delay_opts, T&& f, T2&& after_f) {
  return every(tag, delay_opts, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(const std::string& tag, double delay, T&& f, T2&& after_f) {
  return every(tag, std::vector{delay}, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(const std::vector<double>& delay_opts, std::int64_t count, T&& f, T2&& after_f) {
  return every(rnd::base58(11), delay_opts, count, std::forward<T>(f), std::forward<T2>(after_f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(double delay, std::int64_t count, T&& f, T2&& after_f) {
  return every(rnd::base58(11), std::vector{delay}, count, std::forward<T>(f), std::forward<T2>(after_f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(const std::vector<double>& delay_opts, T&& f, T2&& after_f) {
  return every(rnd::base58(11), delay_opts, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<EveryFunc T, AfterFunc T2>
std::string TimerMgr::every(double delay, T&& f, T2&& after_f) {
  return every(rnd::base58(11), std::vector{delay}, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(const std::string& tag, const std::vector<double>& delay_opts, std::int64_t count, T&& f,
                            T2&& after_f) {
  timers_[tag] = std::make_unique<UntilTimer<T, T2>>(delay_opts, count, std::forward<T>(f), std::forward<T2>(after_f));
  return tag;
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(const std::string& tag, double delay, std::int64_t count, T&& f, T2&& after_f) {
  return until(tag, std::vector{delay}, count, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(const std::string& tag, const std::vector<double>& delay_opts, T&& f, T2&& after_f) {
  return until(tag, delay_opts, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(const std::string& tag, double delay, T&& f, T2&& after_f) {
  return until(tag, std::vector{delay}, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(const std::vector<double>& delay_opts, std::int64_t count, T&& f, T2&& after_f) {
  return until(rnd::base58(11), delay_opts, count, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(double delay, std::int64_t count, T&& f, T2&& after_f) {
  return until(rnd::base58(11), std::vector{delay}, count, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(const std::vector<double>& delay_opts, T&& f, T2&& after_f) {
  return until(rnd::base58(11), delay_opts, -1, std::forward<T>(f), std::forward<T2>(after_f));
}

template<UntilFunc T, AfterFunc T2>
std::string TimerMgr::until(double delay, T&& f, T2&& after_f) {
  return until(rnd::base58(11), std::vector{delay}, -1, std::forward<T>(f), std::forward<T2>(after_f));
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

template<typename T, typename T2>
void TimerMgr::EveryTimer<T, T2>::update(double dt) {
  acc += dt;

  if (acc >= delay)
    should_fire = true;
}

template<typename T, typename T2>
void TimerMgr::EveryTimer<T, T2>::fire() {
  f();
  should_fire = false;
  acc -= delay;
  delay = rnd::choose(delay_opts);

  count_acc++;
  if (count_acc == count)
    expired = true;

  if (expired)
    f2();
}

template<typename T, typename T2>
void TimerMgr::UntilTimer<T, T2>::update(double dt) {
  acc += dt;

  if (acc >= delay)
    should_fire = true;
}

template<typename T, typename T2>
void TimerMgr::UntilTimer<T, T2>::fire() {
  bool should_continue = f();
  if (!should_continue)
    expired = true;

  should_fire = false;
  acc -= delay;
  delay = rnd::choose(delay_opts);

  count_acc++;
  if (count_acc == count)
    expired = true;

  if (expired)
    f2();
}
} // namespace imp

IMP_PRAISE_HERMES(imp::TimerMgr);

#endif//IMPERATOR_UTIL_MODULE_TIMER_MGR_HPP
