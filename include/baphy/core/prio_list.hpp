#ifndef BAPHY_CORE_PRIO_LIST_HPP
#define BAPHY_CORE_PRIO_LIST_HPP

#include "baphy/util/helpers.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

// TODO: Clean up the names in this class--lots of indev test names that are really vague and unhelpful

namespace baphy {

template<typename T>
class PrioList {
  struct ListItem_ {
    int id;
    T v;

    ListItem_(int id, T &&v) : id(id), v(std::move(v)) {}

    ListItem_(const ListItem_ &) = delete;
    ListItem_ &operator=(const ListItem_ &) = delete;

    ListItem_(ListItem_ &&other) noexcept = default;
    ListItem_ &operator=(ListItem_ &&other) noexcept = default;
  };

  struct PendingItem_ {
    ListItem_ v;
    std::vector<int> unmet_deps;
    int remaining_unmet_deps;

    PendingItem_(int id, T &&v, const std::vector<int> &unmet_deps)
      : v(id, std::forward<T>(v)), unmet_deps(unmet_deps), remaining_unmet_deps(unmet_deps.size()) {}

    PendingItem_(const PendingItem_ &) = delete;
    PendingItem_ &operator=(const PendingItem_ &) = delete;

    PendingItem_(PendingItem_ &&other) noexcept = default;
    PendingItem_ &operator=(PendingItem_ &&other) noexcept = default;
  };

public:
  PrioList() = default;
  ~PrioList() = default;

  bool add(const std::string &name, std::vector<std::string> &&deps, T &&v);

  std::string debug_stringify() const;

private:
  std::unordered_map<std::string, int> s_to_id_{};
  std::vector<std::string> id_to_s_{};

  std::vector<ListItem_> order_{};
  std::vector<int> idx_{};

  std::unordered_map<int, PendingItem_> pending_{};
  std::unordered_map<int, std::vector<int>> pending_resolve_{};

  int resolve_id_(const std::string &s);
  void resolve_ids_(const std::string &name, std::vector<std::string> &&deps, int &i, std::vector<int> &ds);

  void resolve_pending_(int i);
};

template<typename T>
bool PrioList<T>::add(const std::string &name, std::vector<std::string> &&deps, T &&v) {
  int i;
  std::vector<int> ds;
  resolve_ids_(name, std::forward<std::vector<std::string>>(deps), i, ds);

  // Don't continue if this item has already been added to the list
  if (idx_[i] != -1) {
    // TODO: Log duplicate
    return false;
  }

  // If this item has no unmet dependencies, add it to the end, check if it resolved dependencies
  if (ds.empty()) {
    order_.emplace_back(i, std::forward<T>(v));
    idx_[i] = order_.size() - 1;
    resolve_pending_(i);
    return true;
  }

  // Store this entry to check later
  pending_.emplace(i, PendingItem_(i, std::forward<T>(v), ds));
  for (const auto &d: ds) {
    if (auto it = pending_resolve_.find(d); it == pending_resolve_.end())
      pending_resolve_[d] = {i};
    else
      it->second.emplace_back(i);
  }

  // Emit a warning if this item has a circular dependency
  for (const auto &d: ds)
    if (std::ranges::contains(pending_resolve_[i], d)) {
      // TODO: Log circular warning
      return false;
    }

  return true;
}

template<typename T>
std::string PrioList<T>::debug_stringify() const {
  std::string s = "[";
  for (const auto &[i, li]: enumerate(order_))
    s += fmt::format("{}{}", i != 0 ? ", " : "", id_to_s_[li.id]);
  return s + "]";
}

template<typename T>
int PrioList<T>::resolve_id_(const std::string &s) {
  if (const auto it = s_to_id_.find(s); it != s_to_id_.end())
    return it->second;

  s_to_id_[s] = id_to_s_.size();
  id_to_s_.emplace_back(s);
  idx_.emplace_back(-1);

  return id_to_s_.size() - 1;
}

template<typename T>
void PrioList<T>::resolve_ids_(const std::string &name, std::vector<std::string> &&deps, int &i, std::vector<int> &ds) {
  i = resolve_id_(name);

  for (const auto &s: deps) {
    auto id = resolve_id_(s);
    if (idx_[id] == -1)
      ds.emplace_back(id);
  }
}

template<typename T>
void PrioList<T>::resolve_pending_(int i) {
  std::vector<int> items_to_resolve;
  items_to_resolve.emplace_back(i);

  while (!items_to_resolve.empty()) {
    auto v = items_to_resolve.back();
    items_to_resolve.pop_back();

    for (const auto &k: pending_resolve_[v]) {
      if (idx_[k] != -1)
        continue;

      auto it = pending_.find(k);
      if (it == pending_.end())
        continue;

      auto it2 = std::ranges::find(it->second.unmet_deps, v);
      if (it2 != it->second.unmet_deps.end()) {
        *it2 = -1;
        --it->second.remaining_unmet_deps;
      }

      if (it->second.remaining_unmet_deps == 0) {
        items_to_resolve.emplace_back(it->second.v.id);
        order_.emplace_back(std::move(it->second.v));
        idx_[items_to_resolve.back()] = order_.size() - 1;

        pending_.erase(it);
      }
    }
  }
}

} // namespace baphy

#endif//BAPHY_CORE_PRIO_LIST_HPP
