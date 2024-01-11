#ifndef IMP_CORE_HERMES_IPP
#define IMP_CORE_HERMES_IPP

namespace imp {
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

#endif//IMP_CORE_HERMES_IPP
