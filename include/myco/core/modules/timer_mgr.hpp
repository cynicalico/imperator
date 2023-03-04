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
concept UntilFunc = std::invocable<T> && requires(T f) {
  { f() } -> std::convertible_to<bool>;
};

template<typename T>
concept DuringFunc = std::invocable<T>;

class TimerMgr : public Module<TimerMgr> {
public:
  TimerMgr() = default;
  ~TimerMgr() override = default;

  template<AfterFunc F>
  std::string after(double delay, F &&f);

  template<EveryFunc F>
  std::string every(std::vector<double> &&intervals, int count, F &&f);

  template<EveryFunc F>
  std::string every(std::vector<double> &&intervals, F &&f);

  template<EveryFunc F>
  std::string every(double interval, int count, F &&f);

  template<EveryFunc F>
  std::string every(double interval, F &&f);

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
    AfterTimer_(double delay, F &&f) : Timer_(delay), f_(f) {}

    bool update(double dt) override {
      acc_ += dt;
      return acc_ >= interval_;
    }

    void fire() override {
      f_();
      expired = true;
    }

  private:
    F f_;
  };

  template<EveryFunc F>
  class EveryTimer_ : public Timer_ {
  public:
    EveryTimer_(std::vector<double> &&intervals, int count, F &&f)
        : Timer_(choose(intervals)), intervals_(intervals), max_count(count), f_(f) {}

    bool update(double dt) override {
      acc_ += dt;
      return acc_ >= interval_;
    }

    void fire() override {
      f_();

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

    F f_;
  };

  std::unordered_map<std::string, std::unique_ptr<Timer_>> timers_{};

  void initialize_(const Initialize &e) override;
  void update_(double dt);
};

template<AfterFunc F>
std::string TimerMgr::after(double delay, F &&f) {
  auto tag = base58(11);
  timers_[tag] = std::make_unique<AfterTimer_<F>>(delay, std::forward<F>(f));

  return tag;
}

template<EveryFunc F>
std::string TimerMgr::every(std::vector<double> &&intervals, int count, F &&f) {
  auto tag = base58(11);
  timers_[tag] = std::make_unique<EveryTimer_<F>>(std::forward<std::vector<double>>(intervals), count, std::forward<F>(f));

  return tag;
}

template<EveryFunc F>
std::string TimerMgr::every(std::vector<double> &&intervals, F &&f) {
  return every(std::forward<std::vector<double>>(intervals), -1, std::forward<F>(f));
}

template<EveryFunc F>
std::string TimerMgr::every(double interval, int count, F &&f) {
  std::vector<double> intervals = {interval};
  return every(std::forward<std::vector<double>>(intervals), count, std::forward<F>(f));
}

template<EveryFunc F>
std::string TimerMgr::every(double interval, F &&f) {
  std::vector<double> intervals = {interval};
  return every(std::forward<std::vector<double>>(intervals), -1, std::forward<F>(f));
}

} // namespace myco

MYCO_DECLARE_MODULE(myco::TimerMgr);
