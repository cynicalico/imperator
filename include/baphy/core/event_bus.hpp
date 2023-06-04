#ifndef BAPHY_CORE_EVENT_BUS_H
#define BAPHY_CORE_EVENT_BUS_H

#include "baphy/core/prio_list.hpp"
#include "baphy/util/type_id.hpp"
#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace baphy {

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

class EventBus {
public:
  template<typename T>
  static void sub(const std::string &name, std::vector<std::string> &&deps, const Receiver<T> &&recv) {
    while (type_id<T> >= receivers_.size())
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
    if (type_id<T> < receivers_.size())
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

} // namespace baphy

#endif//BAPHY_CORE_EVENT_BUS_H
