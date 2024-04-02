#ifndef IMPERATOR_CORE_EVENT_BUS_H
#define IMPERATOR_CORE_EVENT_BUS_H

#include "imperator/core/events.h"
#include "imperator/ds/prio_list.h"
#include "imperator/module/module_mgr.h"
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace imp {
class EventBus final : public Module<EventBus> {
public:
  explicit EventBus(std::weak_ptr<ModuleMgr> module_mgr);

  template <typename T>
  using Receiver = std::function<void(const T&)>;

  template <typename T>
  void presub_cache(const std::string& name);

  template <typename T>
  void sub(const std::string& name, std::vector<std::string>&& deps, Receiver<T>&& recv);

  template <typename T>
  void sub(const std::string& name, Receiver<T>&& recv);

  template <typename T, typename... Args>
  void send(Args&&... args);

  template <typename T, typename... Args>
  void send_nowait(Args&&... args);

  template <typename T, typename... Args>
  void send_nowait_rev(Args&&... args);

  template <typename T>
  void poll(const std::string& name);

  template <typename T>
  std::vector<std::string> get_prio();

  template <typename T>
  bool has_pending();

  template <typename T>
  std::vector<typename PrioList<T>::PendingItemInfo> get_pending();

private:
  template <typename T>
  PrioList<Receiver<T>>& receivers_();

  template <typename T>
  std::unordered_map<std::string, std::vector<T>>& buffers_();

  template <typename T>
  void check_create_buffer_(const std::string& name);
};

template <typename T>
void EventBus::presub_cache(const std::string& name) {
  check_create_buffer_<T>(name);
}

template <typename T>
void EventBus::sub(const std::string& name, std::vector<std::string>&& deps, Receiver<T>&& recv) {
  auto& receivers = receivers_<T>();
  receivers.add(name, std::forward<std::vector<std::string>>(deps), std::forward<Receiver<T>>(recv));
  check_create_buffer_<T>(name);
}

template <typename T>
void EventBus::sub(const std::string& name, Receiver<T>&& recv) {
  sub(name, {}, std::forward<Receiver<T>>(recv));
}

template <typename T, typename... Args>
void EventBus::send(Args&&... args) {
  auto payload = T{std::forward<Args>(args)...};
  for (auto& buffers = buffers_<T>(); auto& p : buffers) {
    p.second.emplace_back(payload);
  }
}

template <typename T, typename... Args>
void EventBus::send_nowait(Args&&... args) {
  auto payload = T{std::forward<Args>(args)...};
  for (auto& receivers = receivers_<T>(); const auto& r : receivers) {
    r(payload);
  }
}

template <typename T, typename... Args>
void EventBus::send_nowait_rev(Args&&... args) {
  auto payload = T{std::forward<Args>(args)...};
  for (auto& receivers = receivers_<T>(); const auto& r : receivers | std::views::reverse) {
    r(payload);
  }
}

template <typename T>
void EventBus::poll(const std::string& name) {
  auto& buffers = buffers_<T>();
  if (auto it = buffers.find(name); it != buffers.end()) {
    for (auto& r = receivers_<T>()[name]; const auto& payload : it->second) {
      r(payload);
    }
    it->second.clear();
  }
}

template <typename T>
std::vector<std::string> EventBus::get_prio() {
  auto ret = std::vector<std::string>{};
  for (auto& receivers = receivers_<T>(); const auto& p : receivers)
    ret.emplace_back(receivers.name_from_id(p.id));
  return ret;
}

template <typename T>
bool EventBus::has_pending() {
  return receivers_<T>().has_pending();
}

template <typename T>
std::vector<typename PrioList<T>::PendingItemInfo> EventBus::get_pending() {
  return receivers_<T>().get_pending();
}

template <typename T>
PrioList<EventBus::Receiver<T>>& EventBus::receivers_() {
  static PrioList<Receiver<T>> r{};
  return r;
}

template <typename T>
std::unordered_map<std::string, std::vector<T>>& EventBus::buffers_() {
  static std::unordered_map<std::string, std::vector<T>> b{};
  return b;
}

template <typename T>
void EventBus::check_create_buffer_(const std::string& name) {
  if (auto& buffers = buffers_<T>(); !buffers.contains(name)) {
    buffers[name] = std::vector<T>{};
  }
}
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::EventBus);

#endif//IMPERATOR_CORE_EVENT_BUS_H
