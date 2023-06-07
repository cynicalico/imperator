#ifndef BAPHY_CORE_ENGINE_HPP
#define BAPHY_CORE_ENGINE_HPP

#include "baphy/core/module/application.hpp"
#include "baphy/core/module/dear_imgui.hpp"
#include "baphy/core/module/gfx_context.hpp"
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
  baphy::log_platform();

  module_mgr_->create<Application, T>();
  module_mgr_->create<DearImgui>();
  module_mgr_->create<GfxContext>();
  module_mgr_->create<Window>();

  EventBus::send_nowait<EInitialize>(window_open_params);
  EventBus::send_nowait<EStartApplication>();

  framecounter_.reset();
  while (!received_shutdown_) {
    EventBus::send_nowait<EUpdate>(framecounter_.dt());
    if (received_shutdown_)
      break;

    EventBus::send_nowait<EStartFrame>();
    EventBus::send_nowait<EDraw>();
    EventBus::send_nowait<EEndFrame>();

    framecounter_.update();
  }
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Engine);

#endif //BAPHY_CORE_ENGINE_HPP
