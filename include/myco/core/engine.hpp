#pragma once

#include "myco/util/log.hpp"

#include "myco/core/modules/application.hpp"
#include "myco/core/modules/input_mgr.hpp"
#include "myco/core/modules/module.hpp"
#include "myco/core/modules/timer_mgr.hpp"
#include "myco/core/modules/window.hpp"
#include "myco/util/time.hpp"

#include <type_traits>

namespace myco {

class Engine : public std::enable_shared_from_this<Engine> {
public:
  FrameCounter frame_counter{};

  Engine();
  ~Engine();

  template<class T, typename... Args> requires std::derived_from<T, ModuleI>
  void add_module(const Args &&...args);

  template<class T> requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> get_module() const;

  template<class T> requires std::derived_from<T, Application>
  void run(const WindowOpenParams &window_open_params);

private:
  bool received_shutdown_{false};
  std::unordered_map<std::string, std::shared_ptr<ModuleI>> modules_{};

  template<class T, class TR, typename... Args> requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
  void add_module_(const Args &&...args);
};

template<class T, typename... Args> requires std::derived_from<T, ModuleI>
void Engine::add_module(const Args &&...args) {
  add_module_<T, T>(std::forward<Args>(args)...);
}

template<class T, class TR, typename... Args> requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
void Engine::add_module_(const Args &&...args) {
  modules_[ModuleInfo<T>::name] = std::shared_ptr<ModuleI>(new TR(std::forward<Args>(args)...));
  MYCO_LOG_DEBUG("Added module {}", modules_[ModuleInfo<T>::name]->name);
}

template<class T> requires std::derived_from<T, ModuleI>
std::shared_ptr<T> Engine::get_module() const {
  return std::dynamic_pointer_cast<T>(modules_.at(ModuleInfo<T>::name));
}

template<class T> requires std::derived_from<T, Application>
void Engine::run(const WindowOpenParams &window_open_params) {
  add_module<Window>();
  add_module<InputMgr>();
  add_module<TimerMgr>();
  add_module_<Application, T>();
  Scheduler::send_nowait<Initialize>(shared_from_this());

  Scheduler::send_nowait<StartApplication>(window_open_params);

  frame_counter.reset();
  while (!received_shutdown_) {
    Scheduler::send_nowait<Update>(frame_counter.dt());

    Scheduler::send_nowait<StartFrame>();
    Scheduler::send_nowait<Draw>();
    Scheduler::send_nowait<EndFrame>();

    frame_counter.update();
  }

  Scheduler::send_nowait<ReleaseEngine>();
}

} // namespace myco
