#include "myco/util/time.hpp"

namespace myco {

Stopwatch::Stopwatch() {
  start();
}

void Stopwatch::start() {
  start_ = time_nsec();
}

void Stopwatch::stop() {
  end_ = time_nsec();
}

std::uint64_t Stopwatch::elapsed_nsec() const {
  return end_ - start_;
}

double Stopwatch::elapsed_usec() const {
  return (end_ - start_) / 1e3;
}

double Stopwatch::elapsed_msec() const {
  return (end_ - start_) / 1e6;
}

double Stopwatch::elapsed_sec() const {
  return (end_ - start_) / 1e9;
}

Ticker::Ticker(double interval) : interval_(static_cast<std::uint64_t>(interval * 1e9)) {
  start_ = time_nsec();
  last_ = start_;
}

void Ticker::reset() {
  start_ = time_nsec();
  last_ = start_;
  dt_ = 0;
  acc_ = 0;
}

std::uint64_t Ticker::tick() {
  auto now = time_nsec();
  dt_ = now - last_;
  last_ = now;

  std::uint64_t tick_count = 0;

  if (interval_ > 0) {
    acc_ += dt_;
    while (acc_ >= interval_) {
      acc_ -= interval_;
      tick_count++;
    }
  }

  return tick_count;
}

double Ticker::dt_nsec() {
  return dt_;
}

double Ticker::dt_usec() {
  return dt_ / 1e3;
}

double Ticker::dt_msec() {
  return dt_ / 1e6;
}

double Ticker::dt_sec() {
  return dt_ / 1e9;
}

std::uint64_t Ticker::elapsed_nsec() const {
  return last_ - start_;
}

double Ticker::elapsed_usec() const {
  return (last_ - start_) / 1e3;
}

double Ticker::elapsed_msec() const {
  return (last_ - start_) / 1e6;
}

double Ticker::elapsed_sec() const {
  return (last_ - start_) / 1e9;
}

FrameCounter::FrameCounter(double interval) {
  start_time_ = time_nsec();
  user_ticker_ = Ticker(interval);
}

void FrameCounter::reset() {
  start_time_ = time_nsec();
  timestamps_.clear();
  dts_.clear();

  user_ticker_.reset();
  ticker_.reset();
  averager_ = EMA(1.0);
}

std::uint64_t FrameCounter::update() {
  timestamps_.emplace_back(time_nsec());
  if (timestamps_.size() == 1)
    dts_.emplace_back(static_cast<double>(timestamps_.back() - start_time_) / 1e9);
  else
    dts_.emplace_back(static_cast<double>(timestamps_.back() - timestamps_[timestamps_.size() - 2]) / 1e9);

  // Keep size >= 2 otherwise dt will give bad results on pauses longer than 1s
  while (timestamps_.size() > 2 && timestamps_.back() - timestamps_.front() > 1'000'000'000) {
    timestamps_.pop_front();
    dts_.pop_front();
  }

  averager_.update(static_cast<double>(timestamps_.size()));
  if (ticker_.tick())
    averager_.alpha = 2.0 / (1.0 + static_cast<double>(timestamps_.size()));

  return user_ticker_.tick();
}

double FrameCounter::fps() const {
  return averager_.value();
}

double FrameCounter::dt() const {
  if (dts_.empty())
    return 0.0;

  return dts_.back();
}

std::vector<double> FrameCounter::dts_vec() const {
  return {dts_.begin(), dts_.end()};
}

} // namespace myco
