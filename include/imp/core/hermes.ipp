#ifndef IMP_CORE_HERMES_IPP
#define IMP_CORE_HERMES_IPP

namespace imp {
template<typename T>
void Hermes::presub_cache(const std::string& name) {
  check_create_buffer_<T>(name);
}

template<typename T>
void Hermes::sub(const std::string& name, std::vector<std::string>&& deps, Receiver<T>&& recv) {
  const std::lock_guard lock2(receiver_mutex_);

  auto& receivers = receivers_<T>;
  receivers.add(name, std::forward<std::vector<std::string>>(deps), std::forward<Receiver<T>>(recv));
  check_create_buffer_<T>(name);
}

template<typename T>
void Hermes::sub(const std::string& name, Receiver<T>&& recv) {
  sub<T>(name, {}, std::forward<Receiver<T>>(recv));
}

template<typename T, typename... Args>
void Hermes::send(Args&&... args) {
  const std::lock_guard lock1(buffer_mutex_);

  auto pay = T{std::forward<Args>(args)...};
  for (auto& p: buffers_<T>) {
    p.second.emplace_back(pay);
  }
}

template<typename T, typename... Args>
void Hermes::send_nowait(Args&&... args) {
  const std::lock_guard lock2(receiver_mutex_);

  auto pay = T{std::forward<Args>(args)...};
  for (const auto& r: receivers_<T>) {
    r(pay);
  }
}

template<typename T, typename... Args>
void Hermes::send_nowait_rev(Args&&... args) {
  const std::lock_guard lock2(receiver_mutex_);

  auto pay = T{std::forward<Args>(args)...};
  for (const auto& r: receivers_<T> | std::views::reverse) {
    r(pay);
  }
}

template<typename T>
void Hermes::poll(const std::string& name) {
  const std::lock_guard lock1(buffer_mutex_);
  const std::lock_guard lock2(receiver_mutex_);

  auto& b = buffers_<T>;
  if (auto it = b.find(name); it != b.end()) {
    auto& r = receivers_<T>[name];
    for (const auto& p: it->second) {
      r(p);
    }
    it->second.clear();
  }
}

template<typename T>
std::vector<std::string> Hermes::get_prio() {
  const std::lock_guard lock(receiver_mutex_);

  auto ret = std::vector<std::string>{};
  for (const auto& p: receivers_<T>)
    ret.emplace_back(receivers_<T>.name_from_id(p.id));
  return ret;
}

template<typename T>
bool Hermes::has_pending() {
  const std::lock_guard lock(receiver_mutex_);
  return receivers_<T>.has_pending();
}

template<typename T>
std::vector<PendingItemInfo> Hermes::get_pending() {
  const std::lock_guard lock(receiver_mutex_);
  return receivers_<T>.get_pending();
}

template<typename T>
void Hermes::check_create_buffer_(const std::string& name) {
  const std::lock_guard lock1(buffer_mutex_);

  auto& buffers = buffers_<T>;
  if (!buffers.contains(name)) {
    buffers[name] = std::vector<T>{};
  }
}
} // namespace imp

#endif//IMP_CORE_HERMES_IPP
