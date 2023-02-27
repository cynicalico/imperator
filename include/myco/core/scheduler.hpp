#pragma once

#include "myco/util/log.hpp"

#include "myco/core/msgs.hpp"
#include "myco/util/helpers.hpp"

#include <any>
#include <limits>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace myco {

template<typename T>
class PrioList {
  static const std::size_t MAX_SIZE_T;

  struct ListItem_ {
    std::size_t id;
    T v;

    ListItem_(std::size_t id, T &&v)
        : id(id), v(std::move(v)) {}

    ListItem_(const ListItem_ &) = delete;
    ListItem_ &operator=(const ListItem_ &) = delete;

    ListItem_(ListItem_ &&other) noexcept = default;
    ListItem_ &operator=(ListItem_ &&other) noexcept = default;
  };

public:
  PrioList() = default;
  ~PrioList() = default;

  PrioList(const PrioList &) = delete;
  PrioList &operator=(const PrioList &) = delete;

  PrioList(PrioList &&other) noexcept = default;
  PrioList &operator=(PrioList &&other) noexcept = default;

  void add(const std::string &name, std::vector<std::string> &&deps, T &&v);

  std::string name_from_id(std::size_t id);

  std::string debug_stringify() const;

  std::vector<ListItem_>::iterator begin() { return order_.begin(); }
  std::vector<ListItem_>::iterator end() { return order_.end(); }

  std::vector<ListItem_>::const_iterator cbegin() { return order_.cbegin(); }
  std::vector<ListItem_>::const_iterator cend() { return order_.cend(); }

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

inline int type_id_seq = 0;
template<typename T> inline const int type_id = type_id_seq++;

template<typename T>
using Receiver = std::function<void(const T &)>;

class ReceiverI {
public:
  ReceiverI() = default;
  virtual ~ReceiverI() = default;

  ReceiverI(const ReceiverI &) = delete;
  ReceiverI &operator=(const ReceiverI &) = delete;

  ReceiverI(ReceiverI &&other) noexcept = default;
  ReceiverI &operator=(ReceiverI &&other) noexcept = default;

  virtual void call(const std::any &e) const {};
};

template<typename T>
class ReceiverWrap : public ReceiverI {
  const Receiver<T> receiver_;
public:
  ReceiverWrap() : receiver_([](const auto &){}) {}
  explicit ReceiverWrap(const Receiver<T> &&receiver)
      : receiver_(std::move(receiver)) {}

  ~ReceiverWrap() override = default;

  ReceiverWrap(const ReceiverWrap &) = delete;
  ReceiverWrap &operator=(const ReceiverWrap &) = delete;

  ReceiverWrap(ReceiverWrap &&other) noexcept = default;
  ReceiverWrap &operator=(ReceiverWrap &&other) noexcept = default;

  void call(const std::any &e) const override {
    receiver_(std::any_cast<T>(e));
  }
};

class Scheduler {
public:
  template<typename T>
  static void sub(const std::string &name, std::vector<std::string> &&deps, const Receiver<T> &&recv) {
    if (type_id<T> >= receivers_.size())
      receivers_.emplace_back();
    receivers_[type_id<T>].add(
        name,
        std::forward<std::vector<std::string>>(deps),
        std::make_unique<ReceiverWrap<T>>(std::forward<const Receiver<T>>(recv))
    );
  }

  template<typename T>
  static void sub(const std::string &name, const Receiver<T> &&recv) {
    sub(name, {}, std::forward<const Receiver<T>>(recv));
  }

  template<typename T, typename... Args>
  static void send_nowait(Args &&... args) {
    for (const auto &p: receivers_[type_id<T>])
      p.v->call(std::any(T{std::forward<Args>(args)...}));
  }

  template<typename T>
  static std::vector<std::string> get_prio() {
    auto ret = std::vector<std::string>{};
    for (const auto &p: receivers_[type_id<T>])
      ret.emplace_back(receivers_[type_id<T>].name_from_id(p.id));
    return ret;
  }

private:
  static std::vector<PrioList<std::unique_ptr<ReceiverI>>> receivers_;
};

template<typename T>
const std::size_t PrioList<T>::MAX_SIZE_T = std::numeric_limits<std::size_t>::max();

template<typename T>
void PrioList<T>::add(const std::string &name, std::vector<std::string> &&deps, T &&v) {
  std::size_t I;
  std::vector<std::size_t> I_deps{};
  hash_get_i_deps_(name, deps, I, I_deps);

  if (idx_[I] != MAX_SIZE_T) {
    MYCO_LOG_WARN("{} already added to PrioList at idx {}", name, idx_[I]);
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

  order_.insert(order_.begin() + min_idx, ListItem_(I, std::forward<T>(v)));

  idx_[I] = min_idx;
  for (std::size_t i = min_idx + 1; i < order_.size(); ++i)
    idx_[order_[i].id]++;
}

template<typename T>
void PrioList<T>::hash_get_i_deps_(
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
void PrioList<T>::find_min_and_max_(
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
bool PrioList<T>::adjust_unmet_(std::vector<std::size_t> &unmet_deps_idxs, std::size_t &min_idx) {
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
std::string PrioList<T>::name_from_id(std::size_t id) {
  return id_to_s_[id];
}

template<typename T>
std::string PrioList<T>::debug_stringify() const {
  std::string s = "[";
  for (const auto &[i, li]: myco::enumerate(order_))
    s += fmt::format("{}{}", i != 0 ? ", " : "", id_to_s_[li.id]);
  return s + "]";
}

} // namespace myco
