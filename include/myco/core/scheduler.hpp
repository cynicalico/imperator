#pragma once

#include "myco/util/helpers.hpp"
#include "myco/util/log.hpp"
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
  };

public:
  PrioList() = default;

  void add(const std::string &name, std::vector<std::string> &&deps, T &&v);

  std::string debug_stringify() const;

  std::vector<ListItem_>::iterator begin() { return order_.begin(); }

  std::vector<ListItem_>::iterator end() { return order_.end(); }

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
  virtual void call(const std::any &e) const {};
};

template<typename T>
class ReceiverWrap : public ReceiverI {
  const Receiver<T> receiver_;
public:
  explicit ReceiverWrap(const Receiver<T> &&receiver)
      : receiver_(std::move(receiver)) {}

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
    receivers_[type_id<T>].add(name, std::forward<std::vector<std::string>>(deps), std::make_unique<ReceiverWrap<T>>(
        std::forward<const Receiver<T>>(recv)));
  }

  template<typename T>
  static void sub(const std::string &name, const Receiver<T> &&recv) {
    sub(name, {}, std::forward<const Receiver<T>>(recv));
  }

  template<typename T, typename... Args>
  static void send(Args &&... args) {
    for (const auto &p: receivers_[type_id<T>])
      p.v->call(std::any(T{std::forward<Args>(args)...}));
  }

private:
  static std::vector<PrioList<std::unique_ptr<ReceiverI>>> receivers_;
};

#include "myco/core/scheduler.ipp"

} // namespace myco