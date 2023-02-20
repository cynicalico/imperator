#pragma once

#include <chrono>
#include <cstddef>

namespace myco {

template<typename T = std::uint64_t>
T time_nsec() {
  using namespace std::chrono;

  auto now = steady_clock::now();
  auto nsecs = duration_cast<nanoseconds>(now.time_since_epoch()).count();
  return static_cast<T>(nsecs);
}

template<typename T = double>
T time_usec() {
  return static_cast<T>(time_nsec<double>() / 1e3);
}

template<typename T = double>
T time_msec() {
  return static_cast<T>(time_nsec<double>() / 1e6);
}

template<typename T = double>
T time_sec() {
  return static_cast<T>(time_nsec<double>() / 1e9);
}

class Stopwatch {
public:
  Stopwatch() { start(); }

  void start() { start_ = time_nsec(); }
  void stop() { end_ = time_nsec(); }

  std::uint64_t elapsed_nsec() const { return end_ - start_; }
  double elapsed_usec() const { return (end_ - start_) / 1e3; }
  double elapsed_msec() const { return (end_ - start_) / 1e6; }
  double elapsed_sec() const { return (end_ - start_) / 1e9; }

private:
  std::uint64_t start_{}, end_{};
};

} // namespace myco
