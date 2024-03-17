#ifndef IMPERATOR_CORE_ENGINE_H
#define IMPERATOR_CORE_ENGINE_H

#include "imperator/core/event_bus.h"
#include "imperator/core/glfw_callbacks.h"
#include "imperator/core/glfw_wrap.h"
#include "imperator/core/type_id.h"
#include "imperator/util/log.h"
#include <memory>
#include <utility>

namespace imp {
template<typename T>
struct ModuleInfo;
} // namespace imp

#define IMPERATOR_DECLARE_MODULE(module)      \
  template<> struct imp::ModuleInfo<module> { \
    static constexpr auto name = #module;     \
  }

namespace imp {
class Engine;

class Module {
public:
  std::shared_ptr<Engine> imp_engine;
  std::string imp_module_name;

  Module(std::shared_ptr<Engine> engine, std::string name);

  template<typename T>
  explicit Module(std::shared_ptr<Engine> engine);

  virtual ~Module();
};

template<typename T>
Module::Module(std::shared_ptr<Engine> engine)
  : Module(std::move(engine), ModuleInfo<T>::name) {}

inline Module::Module(std::shared_ptr<Engine> engine, std::string name)
  : imp_engine(std::move(engine)), imp_module_name(std::move(name)) {
  IMPERATOR_LOG_DEBUG("Module created: {}", imp_module_name);
}

inline Module::~Module() {
  IMPERATOR_LOG_DEBUG("Module destroyed: {}", imp_module_name);
}

class Application;

class Engine : public Module<Engine>, public std::enable_shared_from_this<Engine> {
public:
  static std::shared_ptr<Engine> create();

  template<typename T>
    requires std::derived_from<T, Application>
  void run_application(WindowOpenParams params);

  template<class T, class TR, typename... Args>
    requires std::derived_from<T, Module> && std::derived_from<TR, T>
  std::shared_ptr<T> create_module(Args&&... args);

  template<typename T, typename... Args>
    requires std::derived_from<T, Module>
  std::shared_ptr<T> create_module(Args&&... args);

  template<typename T>
    requires std::derived_from<T, Module>
  std::shared_ptr<T> get_module() const;

  /********************
   * EVENT BUS FACADE *
   ********************/

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
  std::vector<std::shared_ptr<Module>> modules_{};

  void mainloop_(WindowOpenParams params, const std::function<void()>& application_creation_function);
};

template<typename T>
  requires std::derived_from<T, Application>
void Engine::run_application(WindowOpenParams params) {
  register_glfw_error_callback();
  set_global_user_pointer(shared_from_this().get());

  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  int major, minor, rev;
  glfwGetVersion(&major, &minor, &rev);
  IMPERATOR_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, rev);

  mainloop_(params, [&] { create_module<Application, T>(); });

  clear_global_user_pointer();
  glfwTerminate();
  IMPERATOR_LOG_DEBUG("Terminated GLFW");
}

template<class T, class TR, typename... Args>
  requires std::derived_from<T, Module> && std::derived_from<TR, T>
std::shared_ptr<T> Engine::create_module(Args&&... args) {
  auto idx = type_id<T>();
  while (modules_.size() <= idx)
    modules_.emplace_back();

  modules_[idx] = std::make_shared<TR>(shared_from_this(), std::forward<Args>(args)...);
  return get_module<T>();
}

template<typename T, typename... Args>
  requires std::derived_from<T, Module>
std::shared_ptr<T> Engine::create_module(Args&&... args) {
  return create_module<T, T>(std::forward<Args>(args)...);
}

template<typename T>
  requires std::derived_from<T, Module>
std::shared_ptr<T> Engine::get_module() const {
  auto idx = type_id<T>();
  return std::static_pointer_cast<T>(modules_[idx]);
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

IMPERATOR_DECLARE_MODULE(imp::Engine);

#endif//IMPERATOR_CORE_ENGINE_H
