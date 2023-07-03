#ifndef BAPHY_CORE_ENGINE_HPP
#define BAPHY_CORE_ENGINE_HPP

#include "baphy/core/module/application.hpp"
#include "baphy/core/module/dear_imgui.hpp"
#include "baphy/core/module/input_mgr.hpp"
#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/util/module/debug_overlay.hpp"
#include "baphy/util/module/thread_pool.hpp"
#include "baphy/util/module/timer_mgr.hpp"
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
  EventBus::presub_for_cache<ELogMsg>(EPI<DebugOverlay>::name);

  baphy::log_platform();

  module_mgr_->create<Application, T>();
  module_mgr_->create<DearImgui>();
  module_mgr_->create<InputMgr>();
  module_mgr_->create<Window>();

  module_mgr_->create<GfxContext>();
  module_mgr_->create<ShaderMgr>();

  module_mgr_->create<DebugOverlay>();
  module_mgr_->create<ThreadPool>();
  module_mgr_->create<TimerMgr>();

  EventBus::send_nowait<EInitialize>(window_open_params);
  EventBus::send_nowait<EStartApplication>();

  framecounter_.reset();
  while (!received_shutdown_) {
    EventBus::send_nowait<EUpdate>(framecounter_.dt(), framecounter_.fps(), framecounter_.ts());
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

#endif//BAPHY_CORE_ENGINE_HPP
