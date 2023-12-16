#ifndef IMPERATOR_CORE_HERMES_HPP
#define IMPERATOR_CORE_HERMES_HPP

#include "imperator/core/hermes_payloads.hpp"
#include "imperator/core/prio_list.hpp"
#include "imperator/core/type_id.hpp"
#include "imperator/util/log.hpp"
#include <any>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace imp {
template<typename T>
struct EPI;

template<typename T>
using Receiver = std::function<void(const T&)>;

class ReceiverI {
public:
  ReceiverI() = default;

  virtual ~ReceiverI() = default;

  ReceiverI(const ReceiverI&) = delete;
  ReceiverI& operator=(const ReceiverI&) = delete;

  ReceiverI(ReceiverI&& other) noexcept = default;
  ReceiverI& operator=(ReceiverI&& other) noexcept = default;

  virtual void call(const std::any& e) const {};
};

template<typename T>
class ReceiverWrap final : public ReceiverI {
  const Receiver<T> receiver_;

public:
  ReceiverWrap() : receiver_([](const auto&) {}) {}

  explicit ReceiverWrap(const Receiver<T>&& receiver)
    : receiver_(std::move(receiver)) {}

  ~ReceiverWrap() override = default;

  ReceiverWrap(const ReceiverWrap&) = delete;
  ReceiverWrap& operator=(const ReceiverWrap&) = delete;

  ReceiverWrap(ReceiverWrap&& other) noexcept = default;
  ReceiverWrap& operator=(ReceiverWrap&& other) noexcept = default;

  void call(const std::any& e) const override;
};

class Hermes {
public:
  template<typename T>
  static void presub_cache(const std::string& name);

  template<typename T>
  static void sub(const std::string& name, std::vector<std::string>&& deps, const Receiver<T>&& recv);

  template<typename T>
  static void sub(const std::string& name, const Receiver<T>&& recv);

  template<typename T, typename... Args>
  static void send(Args&&... args);

  template<typename T, typename... Args>
  static void send_nowait(Args&&... args);

  // Call the receivers in reverse-dependency order
  // Useful if you need things to shut down correctly
  template<typename T, typename... Args>
  static void send_nowait_rev(Args&&... args);

  template<typename T>
  static void poll(const std::string& name);

  template<typename T>
  static std::vector<std::string> get_prio();

  template<typename T>
  static bool has_pending();

  template<typename T>
  static std::vector<PendingItemInfo> get_pending();

private:
  template<typename T>
  static void check_create_buffer_(const std::string& name);

  static std::vector<PrioList<std::unique_ptr<ReceiverI>>> receivers_;
  static std::vector<std::unordered_map<std::string, std::vector<std::any>>> buffers_;

  static std::recursive_mutex receiver_mutex_;
  static std::recursive_mutex buffer_mutex_;
};

template<typename T>
void ReceiverWrap<T>::call(const std::any& e) const {
  receiver_(std::any_cast<T>(e));
}

template<typename T>
void Hermes::presub_cache(const std::string& name) {
  check_create_buffer_<T>(name);
}

template<typename T>
void Hermes::sub(const std::string& name, std::vector<std::string>&& deps, const Receiver<T>&& recv) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

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
void Hermes::sub(const std::string& name, const Receiver<T>&& recv) {
  sub(name, {}, std::forward<const Receiver<T>>(recv));
}

template<typename T, typename... Args>
void Hermes::send(Args&&... args) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(buffer_mutex_);

  if (e_idx < buffers_.size()) {
    auto pay = std::any(T{std::forward<Args>(args)...});
    for (auto& p: buffers_[e_idx])
      p.second.emplace_back(pay);
  }
}

template<typename T, typename... Args>
void Hermes::send_nowait(Args&&... args) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  if (e_idx < receivers_.size()) {
    auto pay = std::any(T{std::forward<Args>(args)...});
    for (const auto& p: receivers_[type_id<T>()])
      p->call(pay);
  }
}

template<typename T, typename... Args>
void Hermes::send_nowait_rev(Args&&... args) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  if (e_idx < receivers_.size()) {
    auto pay = std::any(T{std::forward<Args>(args)...});
    for (const auto& p: receivers_[type_id<T>()] | std::views::reverse)
      p->call(pay);
  }
}

template<typename T>
void Hermes::poll(const std::string& name) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(buffer_mutex_);

  for (const auto& pay: buffers_[e_idx][name])
    receivers_[e_idx][name]->call(pay);
  buffers_[e_idx][name].clear();
}

template<typename T>
std::vector<std::string> Hermes::get_prio() {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  auto ret = std::vector<std::string>{};
  for (const auto& p: receivers_[e_idx])
    ret.emplace_back(receivers_[e_idx].name_from_id(p.id));
  return ret;
}

template<typename T>
bool Hermes::has_pending() {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  return receivers_[e_idx].has_pending();
}

template<typename T>
std::vector<PendingItemInfo> Hermes::get_pending() {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  return receivers_[e_idx].get_pending();
}

template<typename T>
void Hermes::check_create_buffer_(const std::string& name) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(buffer_mutex_);

  while (e_idx >= buffers_.size())
    buffers_.emplace_back();

  if (!buffers_[e_idx].contains(name))
    buffers_[e_idx][name] = std::vector<std::any>{};
}
} // namespace imp

#define IMPERATOR_PRAISE_HERMES(module)   \
  template<> struct imp::EPI<module> {    \
    static constexpr auto name = #module; \
  }

#endif//IMPERATOR_CORE_HERMES_HPP
