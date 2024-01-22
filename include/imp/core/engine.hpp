#ifndef IMP_CORE_ENGINE_HPP
#define IMP_CORE_ENGINE_HPP

#include "imp/core/module/application.hpp"
#include "imp/core/module/cursor_mgr.hpp"
#include "imp/core/module/input_mgr.hpp"
#include "imp/core/module/window.hpp"

#include "imp/core/hermes.hpp"
#include "imp/core/module_mgr.hpp"

#include "imp/gfx/module/dear_imgui.hpp"
#include "imp/gfx/module/gfx_context.hpp"
#include "imp/gfx/module/shader_mgr.hpp"

#include "imp/util/module/debug_overlay.hpp"
#include "imp/util/module/timer_mgr.hpp"

#include "imp/util/platform.hpp"
#include "imp/util/time.hpp"

#include <atomic>
#include <concepts>

namespace imp {
class Engine {
public:
  Engine();

  template<typename T>
    requires std::derived_from<T, Application>
  void run_application(const WindowOpenParams& initialize_params);

private:
  FrameCounter frame_counter_{};
  std::atomic_bool received_shutdown_{false};

  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};

  bool check_pending_();
};
}

IMP_PRAISE_HERMES(imp::Engine);

namespace imp {
template<typename T>
  requires std::derived_from<T, Application>
void Engine::run_application(const WindowOpenParams& initialize_params) {
  // Make sure the DebugOverlay gets *all* log messages from the beginning
  Hermes::presub_cache<E_LogMsg>(EPI<DebugOverlay>::name);

  log_platform();

  const auto debug_overlay = module_mgr_->create<DebugOverlay>();

  module_mgr_->create<Window>(initialize_params);
  module_mgr_->create<InputMgr>();
  module_mgr_->create<CursorMgr>();

  module_mgr_->create<GfxContext>(initialize_params);
  module_mgr_->create<ShaderMgr>();
  module_mgr_->create<DearImgui>();

  module_mgr_->create<TimerMgr>();

  module_mgr_->create<Application, T>();

  if (check_pending_()) {
    debug_overlay->lateinit_modules();

    while (!received_shutdown_) {
      Hermes::send_nowait<E_Update>(frame_counter_.dt(), frame_counter_.fps());

      Hermes::send_nowait<E_StartFrame>();
      Hermes::send_nowait<E_Draw>();
      Hermes::send_nowait<E_EndFrame>();

      frame_counter_.update();

      glfwPollEvents();
    }
  }
}
} // namespace imp

#endif//IMP_CORE_ENGINE_HPP
