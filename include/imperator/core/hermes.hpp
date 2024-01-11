#ifndef IMPERATOR_CORE_HERMES_HPP
#define IMPERATOR_CORE_HERMES_HPP

#include "imperator/core/hermes_payloads.hpp"
#include "imperator/core/prio_list.hpp"
#include "imperator/core/type_id.hpp"
#include "imperator/util/log.hpp"
#include <any>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace imp {
template<typename T>
struct EPI;

using Receiver = std::function<void(const std::any&)>;
} // namespace imp

#define IMP_MAKE_RECEIVER(T, f)       \
  [&](const std::any& payload) {  \
    f(std::any_cast<T>(payload)); \
  }

#define IMP_PRAISE_HERMES(module)   \
  template<> struct imp::EPI<module> {    \
    static constexpr auto name = #module; \
  }

namespace imp {
class Hermes {
public:
  template<typename T>
  static void presub_cache(const std::string& name);

  template<typename T>
  static void sub(const std::string& name, std::vector<std::string>&& deps, Receiver&& recv);

  template<typename T>
  static void sub(const std::string& name, Receiver&& recv);

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

  static std::vector<PrioList<Receiver>> receivers_;
  static std::vector<std::unordered_map<std::string, std::vector<std::any>>> buffers_;

  static std::recursive_mutex receiver_mutex_;
  static std::recursive_mutex buffer_mutex_;
};

template<typename T>
void Hermes::presub_cache(const std::string& name) {
  check_create_buffer_<T>(name);
}

template<typename T>
void Hermes::sub(const std::string& name, std::vector<std::string>&& deps, Receiver&& recv) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  while (e_idx >= receivers_.size())
    receivers_.emplace_back();
  receivers_[e_idx].add(
    name,
    std::forward<std::vector<std::string>>(deps),
    std::forward<Receiver>(recv)
  );

  check_create_buffer_<T>(name);
}

template<typename T>
void Hermes::sub(const std::string& name, Receiver&& recv) {
  sub<T>(name, {}, std::forward<Receiver>(recv));
}

template<typename T, typename... Args>
void Hermes::send(Args&&... args) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(buffer_mutex_);

  auto pay = std::any(T{std::forward<Args>(args)...});
  if (e_idx < buffers_.size()) {
    for (auto& p: buffers_[e_idx])
      p.second.emplace_back(pay);
  }
}

template<typename T, typename... Args>
void Hermes::send_nowait(Args&&... args) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  auto pay = std::any(T{std::forward<Args>(args)...});
  if (e_idx < receivers_.size()) {
    for (const auto& p: receivers_[type_id<T>()])
      p(pay);
  }
}

template<typename T, typename... Args>
void Hermes::send_nowait_rev(Args&&... args) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  auto pay = std::any(T{std::forward<Args>(args)...});
  if (e_idx < receivers_.size()) {
    for (const auto& p: receivers_[type_id<T>()] | std::views::reverse)
      p(pay);
  }
}

template<typename T>
void Hermes::poll(const std::string& name) {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(buffer_mutex_);

  for (const auto& pay: buffers_[e_idx][name])
    receivers_[e_idx][name](pay);
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

  if (receivers_.size() > e_idx)
    return receivers_[e_idx].has_pending();
  return false;
}

template<typename T>
std::vector<PendingItemInfo> Hermes::get_pending() {
  auto e_idx = type_id<T>();

  const std::lock_guard lock(receiver_mutex_);

  if (receivers_.size() > e_idx)
    return receivers_[e_idx].get_pending();
  return {};
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

#endif//IMPERATOR_CORE_HERMES_HPP
