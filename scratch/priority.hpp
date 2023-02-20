#include "myco/util/log.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <ranges>
#include <optional>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <string>

template<typename T>
class PrioTracker {
public:
  PrioTracker() = default;

  void add(const std::string &name, T v, std::set<std::string> deps = {});

  std::string debug_stringify();

private:
  std::vector<std::pair<std::string, T>> order_{};
  std::unordered_map<std::string, std::size_t> idxs_{};
  std::unordered_map<std::string, std::set<std::string>> saved_deps_{};
  std::unordered_map<std::string, std::set<std::string>> unmet_deps_{};

  std::tuple<std::size_t, std::optional<std::size_t>, std::set<std::size_t>>
  find_min_and_max_(const std::string &name, const std::set<std::string> &deps);

  std::tuple<bool, std::size_t> bubble_(const std::string &name, std::size_t idx, std::size_t min_idx);
};

template<typename T>
void PrioTracker<T>::add(const std::string &name, T v, std::set<std::string> deps) {
  if (std::find_if(order_.begin(), order_.end(), [name](const auto &p) { return p.first == name; }) != order_.end()) {
    MYCO_LOG_WARN("{} already added", name);
    return;
  }

  if (deps.contains(name))
    deps.erase(name);

  std::size_t min_idx = 0;
  saved_deps_[name] = deps;

  if (!deps.empty()) {
    std::optional<std::size_t> max_idx;
    std::set<std::size_t> unmet_deps_idxs;

    std::tie(min_idx, max_idx, unmet_deps_idxs) = find_min_and_max_(name, deps);
    if (max_idx and max_idx.value() < min_idx) {
      do {
        std::size_t bubble_idx = std::ranges::max(
            unmet_deps_idxs | std::views::filter([min_idx](auto e) { return e < min_idx; }));

        bool success = false;
        std::size_t f_idx = bubble_idx;
        do {
          std::size_t old_f_idx = f_idx;
          std::tie(success, f_idx) = bubble_(name, f_idx, min_idx - 1);

          // This happens when f_idx == min_idx - 1--the element won't move,
          // but the only way this happens is when there's a circular dependency
          if (success && old_f_idx == f_idx) {
            MYCO_LOG_ERROR("Circular dependency");
            return;
          }
        } while (!success && f_idx != min_idx);

        std::tie(min_idx, max_idx, unmet_deps_idxs) = find_min_and_max_(name, deps);
      } while (max_idx and max_idx.value() < min_idx);
    }

    if (!unmet_deps_idxs.empty())
      unmet_deps_[name].clear();
  }

  order_.insert(order_.begin() + min_idx, {name, v});

  idxs_[name] = min_idx;
  for (std::size_t i = min_idx + 1; i < order_.size(); ++i)
    idxs_[order_[i].first]++;
}

template<typename T>
std::tuple<std::size_t, std::optional<std::size_t>, std::set<std::size_t>>
PrioTracker<T>::find_min_and_max_(const std::string &name, const std::set<std::string> &deps) {
  std::size_t min_idx = 0;
  std::optional<std::size_t> max_idx{};
  std::set<std::size_t> unmet_deps_idxs{};

  std::vector<std::size_t> deps_idxs{};
  for (const auto &d: deps) {
    if (idxs_.contains(d))
      deps_idxs.emplace_back(idxs_[d] + 1);
    else
      unmet_deps_[d].emplace(name);
  }

  if (!deps_idxs.empty())
    min_idx = std::ranges::max(deps_idxs);

  for (const auto &n: unmet_deps_[name])
    unmet_deps_idxs.emplace(idxs_[n]);

  if (!unmet_deps_idxs.empty())
    max_idx = std::ranges::min(unmet_deps_idxs);

  return {min_idx, max_idx, unmet_deps_idxs};
}

template<typename T>
std::tuple<bool, std::size_t> PrioTracker<T>::bubble_(const std::string &name, std::size_t idx, std::size_t min_idx) {
  while (idx < min_idx) {
    if (saved_deps_[order_[idx + 1].first].contains(order_[idx].first))
      return {false, idx + 1};  // Try and move the dependency forward enough for us to move this one

    std::swap(order_[idx], order_[idx + 1]);
    idxs_[order_[idx].first]--;
    idxs_[order_[idx + 1].first]++;

    idx++;
  }

  return {true, idx};
}

template<typename T>
std::string PrioTracker<T>::debug_stringify() {
  std::string s = "[";
  std::size_t i = 0;
  for (const auto &e: order_) {
    if (i != 0)
      s += ", ";
    s += fmt::format("{}", e.first);

    i++;
  }
  return s + "]";
}
std::uint64_t time() {
  using namespace std::chrono;

  auto now = steady_clock::now().time_since_epoch();
  return duration_cast<nanoseconds>(now).count();
}
