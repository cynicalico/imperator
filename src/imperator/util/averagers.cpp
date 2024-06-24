#include "imperator/util/averagers.h"

namespace imp {
namespace internal {
double Averager::value() const { return value_; }
} // namespace internal

void CMA::update(double v) { value_ += (v - value_) / ++sample_count_; }

EMA::EMA(double alpha) : alpha(alpha) {}

void EMA::update(double v) { value_ = alpha * v + (1 - alpha) * value_; }

SMA::SMA(std::size_t sample_count) : sample_count(sample_count) {}

void SMA::update(double v) {
    samples_.push(v);
    // Do CMA if we are still filling the buffer
    // Otherwise, do actual SMA by dropping the oldest value
    if (samples_.size() <= sample_count) {
        value_ += ((v - value_) / samples_.size());
    } else {
        value_ += (1.0 / sample_count) * (v - samples_.front());
        samples_.pop();
    }
}
} // namespace imp
