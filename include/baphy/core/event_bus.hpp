#ifndef BAPHY_CORE_EVENT_BUS_H
#define BAPHY_CORE_EVENT_BUS_H

#include "baphy/core/msgs.hpp"
#include "baphy/core/prio_list.hpp"
#include "baphy/util/type_id.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace baphy {

template<typename T>
struct EPI;

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

  virtual void call(const EBase *e) const {};
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

  void call(const EBase *e) const override {
    receiver_(*dynamic_cast<const T *>(e));
  }
};

class EventBus {
public:
  template<typename T>
  static void sub(const std::string &name, std::vector<std::string> &&deps, const Receiver<T> &&recv) {
    auto e_idx = type_id<T>;

    while (e_idx >= receivers_.size())
      receivers_.emplace_back();
    receivers_[e_idx].add(
        name,
        std::forward<std::vector<std::string>>(deps),
        std::make_unique<ReceiverWrap<T>>(std::forward<const Receiver<T>>(recv))
    );

    check_create_buffer_<T>(name);
  }

  template<typename T>
  static void sub(const std::string &name, const Receiver<T> &&recv) {
    sub(name, {}, std::forward<const Receiver<T>>(recv));
  }

  template<typename T>
  static void presub_for_cache(const std::string &name) {
    check_create_buffer_<T>(name);
  }

  template<typename T, typename... Args>
  static void send(Args &&... args) {
    auto e_idx = type_id<T>;

    if (e_idx < buffers_.size()) {
      for (auto &p: buffers_[e_idx])
        p.second.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }
  }

  template<typename T, typename... Args>
  static void send_nowait(Args &&... args) {
    auto e_idx = type_id<T>;

    if (e_idx < receivers_.size()) {
      auto pay = std::make_unique<T>(std::forward<Args>(args)...);
      for (const auto &p: receivers_[type_id<T>])
        p.v->call(pay.get());
    }
  }

  template<typename T>
  static void poll(const std::string &name) {
    auto e_idx = type_id<T>;

    for (auto &pay: buffers_[e_idx][name])
      receivers_[e_idx][name]->call(pay.get());
    buffers_[e_idx][name].clear();
  }

  template<typename T>
  static std::vector<std::string> get_prio() {
    auto e_idx = type_id<T>;

    auto ret = std::vector<std::string>{};
    for (const auto &p: receivers_[e_idx])
      ret.emplace_back(receivers_[e_idx].name_from_id(p.id));
    return ret;
  }

private:
  template<typename T>
  static void check_create_buffer_(const std::string &name) {
    auto e_idx = type_id<T>;

    while (e_idx >= buffers_.size())
      buffers_.emplace_back();

    if (!buffers_[e_idx].contains(name))
      buffers_[e_idx][name] = std::vector<std::unique_ptr<EBase>>{};
  }

  static std::vector<PrioList<std::unique_ptr<ReceiverI>>> receivers_;
  static std::vector<std::unordered_map<std::string, std::vector<std::unique_ptr<EBase>>>> buffers_;
};

} // namespace baphy

#define BAPHY_REGISTER_ENDPOINT(module)   \
  template<> struct baphy::EPI<module> {  \
    static constexpr auto name = #module; \
  }

#endif//BAPHY_CORE_EVENT_BUS_H
