#ifndef BAPHY_UTIL_HELPERS_H
#define BAPHY_UTIL_HELPERS_H

#include <concepts>
#include <optional>
#include <ranges>
#include <string>
#include "nlohmann/json.hpp"

namespace baphy {
namespace internal {

template <std::ranges::range T>
class iterator {
  std::ranges::range_difference_t<T> i{};
  std::ranges::iterator_t<T> iter;

public:
  explicit iterator(std::ranges::iterator_t<T> it): iter(it) {}

  bool operator!=(const iterator &other) const {
    return iter != other.iter;
  }

  void operator++() {
    ++i;
    ++iter;
  }

  auto operator*() const {
    return std::tie(i, *iter);
  }
};

template <std::ranges::range T>
struct iterable_wrapper {
  T iterable;

  auto begin() { return iterator<T>(std::begin(iterable)); }
  auto end() { return iterator<T>(std::end(iterable)); }
};

template<typename R, typename F, typename... Args>
std::optional<R> sto_opt(F &&f, Args &&...args) {
  std::optional<R> v{};
  try {
    v = f(std::forward<Args>(args)...);
  } catch (const std::exception &e) {}
  return v;
}

} // namespace internal

template <std::ranges::range T>
constexpr auto enumerate(T &&iterable) {
  return internal::iterable_wrapper<T>{std::forward<T>(iterable)};
}

template<typename T>
std::optional<double> stod(const T &str, size_t *idx = nullptr) {
  double (*f)(const T &, size_t *) = std::stod;
  return internal::sto_opt<double>(f, str, idx);
}

template<typename T>
std::optional<float> stof(const T &str, size_t *idx = nullptr) {
  float (*f)(const T &, size_t *) = std::stof;
  return internal::sto_opt<float>(f, str, idx);
}

template<typename T>
std::optional<int> stoi(const T &str, std::size_t *idx = nullptr, int base = 10) {
  int (*f)(const T &, size_t *, int) = std::stoi;
  return internal::sto_opt<int>(f, str, idx, base);
}

template<typename T>
std::optional<long> stol(const T &str, std::size_t *idx = nullptr, int base = 10) {
  long (*f)(const T &, size_t *, int) = std::stol;
  return internal::sto_opt<long>(f, str, idx, base);
}

template<typename T>
std::optional<long double> stold(const T &str, size_t *idx = nullptr) {
  long double (*f)(const T &, size_t *) = std::stold;
  return internal::sto_opt<long double>(f, str, idx);
}

template<typename T>
std::optional<long long> stoll(const T &str, std::size_t *idx = nullptr, int base = 10) {
  long long (*f)(const T &, size_t *, int) = std::stoll;
  return internal::sto_opt<long long>(f, str, idx, base);
}

template<typename T>
std::optional<unsigned long> stoul(const T &str, std::size_t *idx = nullptr, int base = 10) {
  unsigned long (*f)(const T &, size_t *, int) = std::stoul;
  return internal::sto_opt<unsigned long>(f, str, idx, base);
}

template<typename T>
std::optional<unsigned long long> stoull(const T &str, std::size_t *idx = nullptr, int base = 10) {
  unsigned long long (*f)(const T &, size_t *, int) = std::stoull;
  return internal::sto_opt<unsigned long long>(f, str, idx, base);
}

template<typename T>
std::optional<T> json_get(const nlohmann::json &j, const std::string &key) {
  if (!j.contains(key))
    return std::nullopt;
  return j[key].get<T>();
}

double lerp(double t, double min, double max);

double normalize(double x, double min, double max);

} // namespace baphy

#endif//BAPHY_UTIL_HELPERS_H
