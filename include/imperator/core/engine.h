#ifndef IMPERATOR_CORE_ENGINE_H
#define IMPERATOR_CORE_ENGINE_H

#include "imperator/core/event_bus.h"
#include "imperator/module/module_mgr.h"
#include "imperator/util/platform.h"
#include <memory>

namespace imp {
class Engine : public std::enable_shared_from_this<Engine> {
public:
  static std::shared_ptr<Engine> create() {
    const auto e = std::make_shared<Engine>();
    e->initialize();

    return e;
  }

  void initialize() {
    log_platform();

    event_bus_ = std::make_shared<EventBus>();
    module_mgr_ = std::make_shared<ModuleMgr>(shared_from_this());

    IMPERATOR_LOG_DEBUG("Engine initialized");
  }

  template<class T, class TR, typename... Args>
    requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
  std::shared_ptr<T> create_module(Args&&... args);

  template<typename T, typename... Args>
    requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> create_module(Args&&... args);

  template<typename T>
    requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> get_module() const;

  template<typename T>
  void presub_cache_event(const std::string& name);

  template<typename T>
  void sub_event(const std::string& name, std::vector<std::string>&& deps, EventBus::Receiver<T>&& recv);

  template<typename T>
  void sub_event(const std::string& name, EventBus::Receiver<T>&& recv);

  template<typename T, typename... Args>
  void send_event(Args&&... args);

  template<typename T, typename... Args>
  void send_event_nowait(Args&&... args);

  template<typename T, typename... Args>
  void send_event_nowait_rev(Args&&... args);

  template<typename T>
  void poll_event(const std::string& name);

private:
  std::shared_ptr<EventBus> event_bus_{nullptr};
  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};
};

template<class T, class TR, typename... Args>
  requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
std::shared_ptr<T> Engine::create_module(Args&&... args) {
  return module_mgr_->create<T, TR>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
  requires std::derived_from<T, ModuleI>
std::shared_ptr<T> Engine::create_module(Args&&... args) {
  return module_mgr_->create<T>(std::forward<Args>(args)...);
}

template<typename T>
  requires std::derived_from<T, ModuleI>
std::shared_ptr<T> Engine::get_module() const {
  return module_mgr_->get<T>();
}

template<typename T>
void Engine::presub_cache_event(const std::string& name) {
  event_bus_->presub_cache<T>(name);
}

template<typename T>
void Engine::sub_event(const std::string& name, std::vector<std::string>&& deps, EventBus::Receiver<T>&& recv) {
  event_bus_->sub<T>(name, std::forward<std::vector<std::string>>(deps), std::forward<EventBus::Receiver<T>>(recv));
}

template<typename T>
void Engine::sub_event(const std::string& name, EventBus::Receiver<T>&& recv) {
  event_bus_->sub<T>(name, {}, std::forward<EventBus::Receiver<T>>(recv));
}

template<typename T, typename... Args>
void Engine::send_event(Args&&... args) {
  event_bus_->send<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
void Engine::send_event_nowait(Args&&... args) {
  event_bus_->send_nowait<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
void Engine::send_event_nowait_rev(Args&&... args) {
  event_bus_->send_nowait_rev<T>(std::forward<Args>(args)...);
}

template<typename T>
void Engine::poll_event(const std::string& name) {
  event_bus_->poll<T>(name);
}
} // namespace imp

#endif//IMPERATOR_CORE_ENGINE_H
