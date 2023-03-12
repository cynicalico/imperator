#pragma once

#include "myco/core/modules/module.hpp"
#include "myco/util/rnd.hpp"

#include <concepts>

namespace myco {

template<typename T>
concept AfterFunc = std::invocable<T>;

template<typename T>
concept EveryFunc = std::invocable<T>;

template<typename T>
concept EveryBoolFunc = std::invocable<T> && requires(T f) {
  { f() } -> std::convertible_to<bool>;
};

template<typename T>
concept DuringFunc = std::invocable<T, double>;

class TimerMgr : public Module<TimerMgr> {
public:
  TimerMgr() = default;
  ~TimerMgr() override = default;

  template<AfterFunc F>
  std::string after(const std::string &tag, double delay, F &&f);

  template<AfterFunc F>
  std::string after(double delay, F &&f);

  template<EveryBoolFunc F>
  std::string every(const std::string &tag, std::vector<double> &&intervals, int count, F &&f);

  template<EveryBoolFunc F>
  std::string every(std::vector<double> &&intervals, int count, F &&f);

  template<EveryBoolFunc F>
  std::string every(const std::string &tag, std::vector<double> &&intervals, F &&f);

  template<EveryBoolFunc F>
  std::string every(std::vector<double> &&intervals, F &&f);

  template<EveryBoolFunc F>
  std::string every(const std::string &tag, double interval, int count, F &&f);

  template<EveryBoolFunc F>
  std::string every(double interval, int count, F &&f);

  template<EveryBoolFunc F>
  std::string every(const std::string &tag, double interval, F &&f);

  template<EveryBoolFunc F>
  std::string every(double interval, F &&f);

  template<EveryFunc F>
  std::string every(const std::string &tag, std::vector<double> &&intervals, int count, F &&f);

  template<EveryFunc F>
  std::string every(std::vector<double> &&intervals, int count, F &&f);

  template<EveryFunc F>
  std::string every(const std::string &tag, std::vector<double> &&intervals, F &&f);

  template<EveryFunc F>
  std::string every(std::vector<double> &&intervals, F &&f);

  template<EveryFunc F>
  std::string every(const std::string &tag, double interval, int count, F &&f);

  template<EveryFunc F>
  std::string every(double interval, int count, F &&f);

  template<EveryFunc F>
  std::string every(const std::string &tag, double interval, F &&f);

  template<EveryFunc F>
  std::string every(double interval, F &&f);

  template<DuringFunc Fd, AfterFunc Fa>
  std::string during(const std::string &tag, double interval, Fd &&fd, Fa &&fa);

  template<DuringFunc Fd, AfterFunc Fa>
  std::string during(double interval, Fd &&fd, Fa &&fa);

  template<DuringFunc F>
  std::string during(const std::string &tag, double interval, F &&f);

  template<DuringFunc F>
  std::string during(double interval, F &&f);

  void pause(const std::string &tag);
  void resume(const std::string &tag);
  void toggle(const std::string &tag);

  void cancel(const std::string &tag);
  void cancel_all();

private:
  class Timer_ {
  public:
    bool paused{false};
    bool expired{false};

    explicit Timer_(double interval) : interval_(interval) {}

    virtual ~Timer_() = default;

    virtual bool update(double dt) = 0;
    virtual void fire() = 0;

  protected:
    double interval_;
    double acc_{0.0};
  };

  template<AfterFunc F>
  class AfterTimer_ : public Timer_ {
  public:
    AfterTimer_(double delay, F &&f) : Timer_(delay), f_(std::forward<F>(f)) {}
    ~AfterTimer_() override = default;

    bool update(double dt) override {
      acc_ += dt;
      return acc_ >= interval_;
    }

    void fire() override {
      f_();
      expired = true;
    }

  private:
    const F f_;
  };

  template<EveryBoolFunc F>
  class EveryTimer_ : public Timer_ {
  public:
    EveryTimer_(std::vector<double> &&intervals, int count, F &&f)
        : Timer_(choose(intervals)), intervals_(intervals), max_count(count), f_(std::forward<F>(f)) {}
    ~EveryTimer_() override = default;

    bool update(double dt) override {
      acc_ += dt;
      return acc_ >= interval_;
    }

    void fire() override {
      expired = !f_();

      acc_ -= interval_;
      interval_ = choose(intervals_);

      count++;
      if (count == max_count)
        expired = true;
    }

  private:
    std::vector<double> intervals_;
    int count{0};
    int max_count;

    const F f_;
  };

  template<DuringFunc Fd, AfterFunc Fa>
  class DuringTimer_ : public Timer_ {
  public:
    DuringTimer_(double interval, Fd &&fd, Fa &&fa)
        : Timer_(interval), fd_(std::forward<Fd>(fd)), fa_(std::forward<Fa>(fa)) {}
    ~DuringTimer_() override = default;

    bool update(double dt) override {
      fire_dt = dt;

      acc_ += dt;
      if (acc_ >= interval_)
        expired = true;

      return true;
    }

    void fire() override {
      if (acc_ < interval_)
        fd_(fire_dt);
      else
        fa_();
    }

  private:
    double fire_dt;

    const Fd fd_;
    const Fa fa_;
  };

  std::unordered_map<std::string, std::unique_ptr<Timer_>> timers_{};

  void initialize_(const Initialize &e) override;
  void update_(double dt);
};

/*********
 * AFTER *
 *********/

template<AfterFunc F>
std::string TimerMgr::after(const std::string &tag, double delay, F &&f) {
  timers_[tag] = std::make_unique<AfterTimer_<F>>(delay, std::forward<F>(f));
  return tag;
}

template<AfterFunc F>
std::string TimerMgr::after(double delay, F &&f) {
  return after(base58(11), delay, std::forward<F>(f));
}

/*********
 * EVERY *
 *********/

template<EveryBoolFunc F>
std::string TimerMgr::every(const std::string &tag, std::vector<double> &&intervals, int count, F &&f) {
  timers_[tag] = std::make_unique<EveryTimer_<F>>(std::forward<std::vector<double>>(intervals), count, std::forward<F>(f));
  return tag;
}

template<EveryBoolFunc F>
std::string TimerMgr::every(std::vector<double> &&intervals, int count, F &&f) {
  return every(base58(11), std::forward<std::vector<double>>(intervals), count, std::forward<F>(f));
}

template<EveryBoolFunc F>
std::string TimerMgr::every(const std::string &tag, std::vector<double> &&intervals, F &&f) {
  return every(tag, std::forward<std::vector<double>>(intervals), -1, std::forward<F>(f));
}

template<EveryBoolFunc F>
std::string TimerMgr::every(std::vector<double> &&intervals, F &&f) {
  return every(base58(11), std::forward<std::vector<double>>(intervals), -1, std::forward<F>(f));
}

template<EveryBoolFunc F>
std::string TimerMgr::every(const std::string &tag, double interval, int count, F &&f) {
  std::vector<double> intervals = {interval};
  return every(tag, std::forward<std::vector<double>>(intervals), count, std::forward<F>(f));
}

template<EveryBoolFunc F>
std::string TimerMgr::every(double interval, int count, F &&f) {
  std::vector<double> intervals = {interval};
  return every(base58(11), std::forward<std::vector<double>>(intervals), count, std::forward<F>(f));
}

template<EveryBoolFunc F>
std::string TimerMgr::every(const std::string &tag, double interval, F &&f) {
  std::vector<double> intervals = {interval};
  return every(tag, std::forward<std::vector<double>>(intervals), -1, std::forward<F>(f));
}

template<EveryBoolFunc F>
std::string TimerMgr::every(double interval, F &&f) {
  std::vector<double> intervals = {interval};
  return every(base58(11), std::forward<std::vector<double>>(intervals), -1, std::forward<F>(f));
}

template<EveryFunc F>
std::string every(const std::string &tag, std::vector<double> &&intervals, int count, F &&f) {
  return every(tag, std::forward<std::vector<double>>(intervals), count, [f]() { f(); return true; });
}

template<EveryFunc F>
std::string TimerMgr::every(std::vector<double> &&intervals, int count, F &&f) {
  return every(base58(11), std::forward<std::vector<double>>(intervals), count, [f]() { f(); return true; });
}

template<EveryFunc F>
std::string every(const std::string &tag, std::vector<double> &&intervals, F &&f) {
  return every(tag, std::forward<std::vector<double>>(intervals), -1, [f](){ f(); return true; });
}

template<EveryFunc F>
std::string TimerMgr::every(std::vector<double> &&intervals, F &&f) {
  return every(base58(11), std::forward<std::vector<double>>(intervals), -1, [f](){ f(); return true; });
}

template<EveryFunc F>
std::string every(const std::string &tag, double interval, int count, F &&f) {
  std::vector<double> intervals = {interval};
  return every(tag, std::forward<std::vector<double>>(intervals), count, [f](){ f(); return true; });
}

template<EveryFunc F>
std::string TimerMgr::every(double interval, int count, F &&f) {
  std::vector<double> intervals = {interval};
  return every(base58(11), std::forward<std::vector<double>>(intervals), count, [f](){ f(); return true; });
}

template<EveryFunc F>
std::string every(const std::string &tag, double interval, F &&f) {
  std::vector<double> intervals = {interval};
  return every(tag, std::forward<std::vector<double>>(intervals), -1, [f](){ f(); return true; });
}

template<EveryFunc F>
std::string TimerMgr::every(double interval, F &&f) {
  std::vector<double> intervals = {interval};
  return every(base58(11), std::forward<std::vector<double>>(intervals), -1, [f](){ f(); return true; });
}

/**********
 * DURING *
 **********/

template<DuringFunc Fd, AfterFunc Fa>
std::string TimerMgr::during(const std::string &tag, double interval, Fd &&fd, Fa &&fa) {
  timers_[tag] = std::make_unique<DuringTimer_<Fd, Fa>>(interval, std::forward<Fd>(fd), std::forward<Fa>(fa));
  return tag;
}

template<DuringFunc Fd, AfterFunc Fa>
std::string TimerMgr::during(double interval, Fd &&fd, Fa &&fa) {
  return during(base58(11), interval, std::forward<Fd>(fd), std::forward<Fa>(fa));
}

template<DuringFunc F>
std::string TimerMgr::during(const std::string &tag, double interval, F &&f) {
  return during(tag, interval, std::forward<F>(f), [](){});
}

template<DuringFunc F>
std::string TimerMgr::during(double interval, F &&f) {
  return during(base58(11), interval, std::forward<F>(f), [](){});
}

} // namespace myco

MYCO_DECLARE_MODULE(myco::TimerMgr);
