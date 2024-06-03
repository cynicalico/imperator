#ifndef IMPERATOR_UTIL_AVERAGERS_H
#define IMPERATOR_UTIL_AVERAGERS_H

#include <cstddef>
#include <queue>

namespace imp {
namespace internal {
class Averager {
public:
    virtual ~Averager() = default;

    virtual void update(double v) = 0;

    double value() const;

protected:
    double value_{0.0};
};
} // namespace internal

class CMA : public internal::Averager {
public:
    CMA() = default;

    void update(double v) override;

private:
    std::size_t sample_count_{0};
};

class EMA : public internal::Averager {
public:
    double alpha{0.0};

    explicit EMA(double alpha);

    void update(double v) override;
};

class SMA : public internal::Averager {
public:
    std::size_t sample_count{0};

    explicit SMA(std::size_t sample_count);

    void update(double v) override;

private:
    std::queue<double> samples_{};
};
} // namespace imp

#endif//IMPERATOR_UTIL_AVERAGERS_H
