#pragma once

#include "pcg_random.hpp"
#include "uuid.h"
#include <cstdint>
#include <concepts>

namespace myco {
namespace internal {

pcg64 &generator();

} // namespace internal

struct seed_data {
  pcg_extras::pcg128_t seed{0};
  pcg_extras::pcg128_t stream{0};
};

seed_data &seed_info();

void reseed();

void seed128(
    std::uint64_t seed_hi, std::uint64_t seed_lo,
    std::uint64_t stream_hi = 0, std::uint64_t stream_lo = 0
);

void seed(std::uint64_t seed, std::uint64_t stream = 0);

void debug_show_seed();

template<typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template<typename T>
using IntDist = std::uniform_int_distribution<T>;

template<typename T>
concept IntDistCompatible =
IsAnyOf<T, short, int, long, long long, unsigned short, unsigned int, unsigned long, unsigned long long>;

template<IntDistCompatible T>
T get(T low, T high) {
  return IntDist<T>(low, high)(internal::generator());
}

template<IntDistCompatible T>
T get(T high) {
  return IntDist<T>(T(0), high)(internal::generator());
}

template<IntDistCompatible T>
T get() {
  return IntDist<T>(
      std::numeric_limits<T>::min(),
      std::numeric_limits<T>::max()
  )(internal::generator());
}

template<typename T>
concept IntDistCompatibleButChar = IsAnyOf<T, char, char8_t, char16_t, char32_t, wchar_t, unsigned char>;

template<IntDistCompatibleButChar T>
T get(T low, T high) {
  return static_cast<T>(get<int>(
      static_cast<int>(low),
      static_cast<int>(high)
  ));
}

template<IntDistCompatibleButChar T>
T get(T high) {
  return static_cast<T>(get<int>(static_cast<int>(high)));
}

template<IntDistCompatibleButChar T>
T get() {
  return static_cast<T>(get<int>(
      static_cast<int>(std::numeric_limits<T>::min()),
      static_cast<int>(std::numeric_limits<T>::max())
  ));
}

template<typename T>
using RealDist = std::uniform_real_distribution<T>;

template<typename T>
concept RealDistCompatible = IsAnyOf<T, float, double, long double>;

template<RealDistCompatible T>
T get(T low, T high) {
  return RealDist<T>(low, high)(internal::generator());
}

template<RealDistCompatible T>
T get(T high) {
  return RealDist<T>(T(0.0), high)(internal::generator());
}

template<RealDistCompatible T>
T get(bool between_min_and_max = false) {
  if (between_min_and_max)
    return RealDist<T>(
        std::numeric_limits<T>::min(),
        std::numeric_limits<T>::max()
    )(internal::generator());
  return RealDist<T>(T(0.0), T(1.0))(internal::generator());
}

using BoolDist = std::bernoulli_distribution;

template<typename T>
concept BoolDistCompatible = IsAnyOf<T, bool>;

template<BoolDistCompatible T>
T get(double chance) {
  return BoolDist(chance)(internal::generator());
}

std::string base58(std::size_t length);

std::string uuidv4();

} // namespace myco
