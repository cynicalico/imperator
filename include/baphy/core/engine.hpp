#ifndef BAPHY_CORE_ENGINE_HPP
#define BAPHY_CORE_ENGINE_HPP

#include "baphy/core/module/application.hpp"
#include "baphy/core/module/module.hpp"
#include "baphy/core/module/window.hpp"
#include "baphy/util/time.hpp"
#include <memory>

namespace baphy {

class Engine {
public:
  Engine();

  template<typename T>
  requires std::derived_from<T, Application>
  void run_application(const WindowOpenParams &window_open_params);

private:
  FrameCounter framecounter_{};
  bool received_shutdown_{false};

  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};
};

template<typename T>
requires std::derived_from<T, Application>
void Engine::run_application(const WindowOpenParams &window_open_params) {
  module_mgr_->create<Window>();
  module_mgr_->create<Application, T>();

  EventBus::send_nowait<EInitialize>();
  EventBus::send_nowait<EStartApplication>(window_open_params);

  framecounter_.reset();
  while (!received_shutdown_) {
    EventBus::send_nowait<EUpdate>(framecounter_.dt());

    EventBus::send_nowait<EStartFrame>();
    EventBus::send_nowait<EDraw>();
    EventBus::send_nowait<EEndFrame>();

    framecounter_.update();
  }
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Engine);

#endif //BAPHY_CORE_ENGINE_HPP
