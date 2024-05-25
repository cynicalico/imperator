#ifndef IMPERATOR_DS_PRIO_LIST_H
#define IMPERATOR_DS_PRIO_LIST_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace imp {
template<typename T>
class PrioList {
  struct PendingItem_ {
    int id;
    T v;

    std::vector<int> unmet_deps;
    // To avoid vector item deletion, we will keep track of how many are not set to -1
    std::size_t remaining_unmet_deps;

    PendingItem_(int id, T &&v, const std::vector<int> &unmet_deps)
        : id(id),
          v(std::forward<T>(v)),
          unmet_deps(unmet_deps),
          remaining_unmet_deps(unmet_deps.size()) {}

    PendingItem_(const PendingItem_ &) = delete;

    PendingItem_ &operator=(const PendingItem_ &) = delete;

    PendingItem_(PendingItem_ &&other) noexcept = default;

    PendingItem_ &operator=(PendingItem_ &&other) noexcept = default;
  };

public:
  struct PendingItemInfo {
    std::string name;
    std::vector<std::string> deps;
  };

  PrioList() = default;

  ~PrioList() = default;

  PrioList(const PrioList &) = delete;

  PrioList &operator=(const PrioList &) = delete;

  PrioList(PrioList &&other) noexcept = default;

  PrioList &operator=(PrioList &&other) noexcept = default;

  T &operator[](const std::string &name);

  const T &operator[](const std::string &name) const;

  bool add(const std::string &name, std::vector<std::string> &&deps, T &&v);

  std::string name_from_id(std::size_t id);

  bool has_pending() const;

  std::vector<PendingItemInfo> get_pending() const;

  typename std::vector<T>::iterator begin() { return ts_.begin(); }

  typename std::vector<T>::iterator end() { return ts_.end(); }

  typename std::vector<T>::const_iterator cbegin() { return ts_.cbegin(); }

  typename std::vector<T>::const_iterator cend() { return ts_.cend(); }

private:
  std::unordered_map<std::string, int> s_to_id_{};
  std::vector<std::string> id_to_s_{};

  // The actual data is stored separately, the value in `ts_`, the ids in `ids_`
  std::vector<T> ts_{};
  std::vector<int> ids_{};

  std::vector<std::size_t> idx_{};

  std::unordered_map<int, PendingItem_> pending_{};
  std::unordered_map<int, std::vector<int>> pending_dep_lookup_{};

  int resolve_id_(const std::string &s);

  void resolve_ids_(const std::string &name, std::vector<std::string> &&deps, int &i, std::vector<int> &ds);

  void resolve_pending_(int id);
};

template<typename T>
T &PrioList<T>::operator[](const std::string &name) {
  return ts_[idx_[s_to_id_[name]]];
}

template<typename T>
const T &PrioList<T>::operator[](const std::string &name) const {
  return ts_.at(idx_.at(s_to_id_.at(name)));
}

template<typename T>
bool PrioList<T>::add(const std::string &name, std::vector<std::string> &&deps, T &&v) {
  int id;
  std::vector<int> dep_ids;
  resolve_ids_(name, std::forward<std::vector<std::string>>(deps), id, dep_ids);

  // Don't continue if this item has already been added to the list
  if (idx_[id] != -1) {
    // TODO: Log duplicate
    return false;
  }

  // If this item has no unmet dependencies, add it to the end, check if it resolved dependencies
  if (dep_ids.empty()) {
    ts_.emplace_back(std::forward<T>(v));
    ids_.emplace_back(id);
    idx_[id] = ts_.size() - 1;
    resolve_pending_(id);
    return true;
  }

  // Store this entry to check later
  pending_.emplace(id, PendingItem_(id, std::forward<T>(v), dep_ids));
  for (const auto &dep: dep_ids) {
    if (auto it = pending_dep_lookup_.find(dep); it == pending_dep_lookup_.end()) {
      pending_dep_lookup_[dep] = {id};
    } else {
      it->second.emplace_back(id);
    }
  }

  // Emit a warning if this item has a circular dependency
  if (std::ranges::all_of(
      dep_ids,
      [&](const auto &d) {
        return std::ranges::contains(pending_dep_lookup_[id], d);
      }
  )) {
    // TODO: Log circular warning
    return false;
  }

  return true;
}

template<typename T>
std::string PrioList<T>::name_from_id(std::size_t id) {
  return id_to_s_[id];
}

template<typename T>
bool PrioList<T>::has_pending() const {
  return !pending_.empty();
}

template<typename T>
std::vector<typename PrioList<T>::PendingItemInfo> PrioList<T>::get_pending() const {
  std::vector<PendingItemInfo> pending_info{};
  for (const auto &[id, i]: pending_) {
    pending_info.emplace_back(id_to_s_[id]);
    for (const auto &id2: i.unmet_deps) {
      pending_info.back().deps.emplace_back(id_to_s_[id2]);
    }
  }

  return pending_info;
}

template<typename T>
int PrioList<T>::resolve_id_(const std::string &s) {
  if (const auto it = s_to_id_.find(s); it != s_to_id_.end()) {
    return it->second;
  }

  s_to_id_[s] = static_cast<int>(id_to_s_.size());
  id_to_s_.emplace_back(s);
  idx_.emplace_back(-1);

  return static_cast<int>(id_to_s_.size()) - 1;
}

template<typename T>
void PrioList<T>::resolve_ids_(const std::string &name, std::vector<std::string> &&deps, int &i, std::vector<int> &ds) {
  i = resolve_id_(name);

  for (const auto &s: deps) {
    auto id = resolve_id_(s);
    if (idx_[id] == -1) {
      ds.emplace_back(id);
    }
  }
}

template<typename T>
void PrioList<T>::resolve_pending_(int id) {
  std::vector<int> ids_to_resolve = {id};

  while (!ids_to_resolve.empty()) {
    auto v = ids_to_resolve.back();
    ids_to_resolve.pop_back();

    for (const auto &dep: pending_dep_lookup_[v]) {
      // Skip this dependent if we've already added it
      if (idx_[dep] != -1) {
        continue;
      }

      auto dep_it = pending_.find(dep);

      auto it = std::ranges::find(dep_it->second.unmet_deps, v);
      if (it != dep_it->second.unmet_deps.end()) {
        *it = -1;
        --dep_it->second.remaining_unmet_deps;
      }

      if (dep_it->second.remaining_unmet_deps == 0) {
        ids_to_resolve.emplace_back(dep_it->second.id);
        ts_.emplace_back(std::move(dep_it->second.v));
        ids_.emplace_back(dep_it->second.id);
        idx_[ids_to_resolve.back()] = ts_.size() - 1;

        pending_.erase(dep_it);
      }
    }
  }
}
} // namespace imp

#endif//IMPERATOR_DS_PRIO_LIST_H
