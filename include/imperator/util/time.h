#ifndef IMPERATOR_UTIL_TIME_H
#define IMPERATOR_UTIL_TIME_H

#include "imperator/util/averagers.h"
#include "fmt/chrono.h"
#include "fmt/format.h"
#include <chrono>
#include <ctime>
#include <deque>
#include <string>

namespace imp {
template <typename S>
std::string timestamp(const S& format) {
  auto t = std::time(nullptr);
  auto lt = fmt::localtime(t);
  auto args = fmt::make_format_args(lt);

  return fmt::vformat(format, args);
}

std::string timestamp();

template <typename T = std::uint64_t>
T time_nsec() {
  using namespace std::chrono;

  auto now = steady_clock::now();
  auto nsecs = duration_cast<nanoseconds>(now.time_since_epoch()).count();
  return static_cast<T>(nsecs);
}

template <typename T = double>
T time_usec() {
  return static_cast<T>(time_nsec<double>() / 1e3);
}

template <typename T = double>
T time_msec() {
  return static_cast<T>(time_nsec<double>() / 1e6);
}

template <typename T = double>
T time_sec() {
  return static_cast<T>(time_nsec<double>() / 1e9);
}

class Stopwatch {
public:
  Stopwatch();

  void start();
  void stop();

  std::uint64_t elapsed_nsec() const;
  double elapsed_usec() const;
  double elapsed_msec() const;
  double elapsed_sec() const;

private:
  std::uint64_t start_{0}, end_{0};
};


class Ticker {
public:
  explicit Ticker(double interval = 0.0);

  void reset();

  std::uint64_t tick();

  double dt_nsec();
  double dt_usec();
  double dt_msec();
  double dt_sec();

  std::uint64_t elapsed_nsec() const;
  double elapsed_usec() const;
  double elapsed_msec() const;
  double elapsed_sec() const;

private:
  std::uint64_t start_{0};
  std::uint64_t last_{0};
  std::uint64_t dt_{0};
  std::uint64_t interval_{0};
  std::uint64_t acc_{0};
};

class FrameCounter {
public:
  explicit FrameCounter(double interval = 0.0);

  void reset();

  std::uint64_t update();

  double fps() const;
  std::uint64_t ts() const;
  double dt() const;

private:
  std::uint64_t start_time_{0};
  std::deque<std::uint64_t> timestamps_{};

  Ticker user_ticker_;
  Ticker ticker_{0.5};
  EMA averager_{1.0};
};
} // namespace imp

#endif//IMPERATOR_UTIL_TIME_H
