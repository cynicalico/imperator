#pragma once

#include "myco/util/helpers.hpp"
#include "myco/util/log.hpp"
#include "fmt/format.h"
#include <limits>
#include <ranges>
#include <string>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>

template<typename T>
class PrioListV5 {
  static const std::size_t MAX_SIZE_T;

  struct ListItem_ {
    std::size_t id;
    T v;
  };

public:
  PrioListV5() = default;

  void add(const std::string &name, T v, std::vector<std::string> &deps);
  void add(const std::string &name, T v);

  std::string debug_stringify() const;

private:
  std::unordered_map<std::string, std::size_t> s_to_id_{};
  std::vector<std::string> id_to_s_{};

  std::vector<ListItem_> order_{};
  std::vector<std::size_t> idx_{};
  std::vector<std::vector<std::size_t>> saved_deps_{};
  std::vector<std::vector<std::size_t>> unmet_deps_{};

  void hash_get_i_deps_(
      const std::string &name,
      std::vector<std::string> &deps,
      std::size_t &I,
      std::vector<std::size_t> &i_deps
  );

  void find_min_and_max_(
      std::size_t I,
      const std::vector<std::size_t> &deps,
      std::size_t &min, std::size_t &max,
      std::vector<std::size_t> &unmet_deps_idxs
  );

  bool adjust_unmet_(std::vector<std::size_t> &unmet_deps_idxs, std::size_t &min_idx);
};

template<typename T>
const std::size_t PrioListV5<T>::MAX_SIZE_T = std::numeric_limits<std::size_t>::max();

template<typename T>
void PrioListV5<T>::add(const std::string &name, T v, std::vector<std::string> &deps) {
  std::size_t I;
  std::vector<std::size_t> I_deps{};
  hash_get_i_deps_(name, deps, I, I_deps);

  if (idx_[I] != MAX_SIZE_T) {
    MYCO_LOG_WARN("{} already added to PrioListV5 at idx {}", name, idx_[I]);
    return;
  }
  saved_deps_[I] = I_deps;

  std::size_t min_idx = 0;
  if (!I_deps.empty()) {
    std::size_t max_idx;
    std::vector<std::size_t> unmet_deps_idxs{};
    find_min_and_max_(I, I_deps, min_idx, max_idx, unmet_deps_idxs);

    if (max_idx < min_idx) {
      std::ranges::sort(unmet_deps_idxs);
      adjust_unmet_(unmet_deps_idxs, min_idx);
    }

    if (!unmet_deps_idxs.empty())
      unmet_deps_[I].clear();
  }

  order_.insert(order_.begin() + min_idx, {I, v});

  idx_[I] = min_idx;
  for (std::size_t i = min_idx + 1; i < order_.size(); ++i)
    idx_[order_[i].id]++;
}

template<typename T>
void PrioListV5<T>::add(const std::string &name, T v) {
  add(name, v, {});
}

template<typename T>
void PrioListV5<T>::hash_get_i_deps_(
    const std::string &name,
    std::vector<std::string> &deps,
    std::size_t &I,
    std::vector<std::size_t> &I_deps
) {
  if (!s_to_id_.contains(name)) {
    s_to_id_[name] = s_to_id_.size();
    id_to_s_.emplace_back(name);
    idx_.emplace_back(MAX_SIZE_T);
    saved_deps_.emplace_back();
    unmet_deps_.emplace_back();
  }
  I = s_to_id_[name];

  for (const auto &d: deps) {
    if (!s_to_id_.contains(d)) {
      s_to_id_[d] = s_to_id_.size();
      id_to_s_.emplace_back(d);
      idx_.emplace_back(MAX_SIZE_T);
      saved_deps_.emplace_back();
      unmet_deps_.emplace_back();
    }
    I_deps.emplace_back(s_to_id_[d]);
  }
}

template<typename T>
void PrioListV5<T>::find_min_and_max_(
    std::size_t I,
    const std::vector<std::size_t> &deps,
    std::size_t &min, std::size_t &max,
    std::vector<std::size_t> &unmet_deps_idxs
) {
  std::vector<std::size_t> deps_idxs{};
  for (const auto &d: deps) {
    if (idx_[d] != MAX_SIZE_T)
      deps_idxs.emplace_back(idx_[d] + 1);
    else
      unmet_deps_[d].emplace_back(I);
  }
  min = deps_idxs.empty() ? 0 : std::ranges::max(deps_idxs);

  for (const auto &n: unmet_deps_[I])
    if (idx_[n] < min)
      unmet_deps_idxs.emplace_back(idx_[n]);
  max = unmet_deps_idxs.empty() ? MAX_SIZE_T : std::ranges::min(unmet_deps_idxs);
}

template<typename T>
bool PrioListV5<T>::adjust_unmet_(std::vector<std::size_t> &unmet_deps_idxs, std::size_t &min_idx) {
  while (!unmet_deps_idxs.empty()) {
    std::size_t idx = unmet_deps_idxs.back();
    unmet_deps_idxs.pop_back();

    while (idx < min_idx - 1) {
      auto &curr = saved_deps_[order_[idx + 1].id];
      if (std::find(curr.begin(), curr.end(), order_[idx].id) != curr.end()) {
        unmet_deps_idxs.emplace_back(idx);
        unmet_deps_idxs.emplace_back(idx + 1);
        break;
      }

      std::swap(order_[idx], order_[idx + 1]);

      idx++;
      idx_[order_[idx].id]++;
      idx_[order_[idx - 1].id]--;
    }

    if (idx == min_idx - 1)
      min_idx--;
  }

  return true;
}

template<typename T>
std::string PrioListV5<T>::debug_stringify() const {
  std::string s = "[";
  for (const auto &[i, li]: myco::enumerate(order_))
    s += fmt::format("{}{}", i != 0 ? ", " : "", id_to_s_[li.id]);
  return s + "]";
}
