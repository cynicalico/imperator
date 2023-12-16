#ifndef IMPERATOR_CORE_ENGINE_HPP
#define IMPERATOR_CORE_ENGINE_HPP

#include "imperator/core/module/application.hpp"
#include "imperator/core/module/input_mgr.hpp"
#include "imperator/core/module/window.hpp"
#include "imperator/core/hermes.hpp"
#include "imperator/core/module_mgr.hpp"
#include "imperator/gfx/module/dear_imgui.hpp"
#include "imperator/gfx/module/gfx_context.hpp"
#include "imperator/util/platform.hpp"
#include "imperator/util/time.hpp"
#include <atomic>
#include <concepts>

namespace imp {
class Engine {
public:
  Engine();

  template<typename T>
    requires std::derived_from<T, Application>
  void run_application(const InitializeParams& initialize_params);

private:
  FrameCounter frame_counter_{};
  std::atomic_bool received_shutdown_{false};

  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};

  bool check_pending_();
};
}

IMPERATOR_PRAISE_HERMES(imp::Engine);

namespace imp {
template<typename T>
  requires std::derived_from<T, Application>
void Engine::run_application(const InitializeParams& initialize_params) {
  log_platform();

  module_mgr_->create<Application, T>();
  module_mgr_->create<DearImgui>();
  module_mgr_->create<InputMgr>();
  module_mgr_->create<GfxContext>();
  module_mgr_->create<Window>();

  Hermes::send_nowait<E_Initialize>(initialize_params);

  if (check_pending_()) {
    while (!received_shutdown_) {
      Hermes::send_nowait<E_Update>(frame_counter_.dt());

      Hermes::send_nowait<E_StartFrame>();
      Hermes::send_nowait<E_Draw>();
      Hermes::send_nowait<E_EndFrame>();

      frame_counter_.update();

      glfwPollEvents();
    }
  }

  Hermes::send_nowait_rev<E_Shutdown>();
}
} // namespace imp

#endif//IMPERATOR_CORE_ENGINE_HPP
