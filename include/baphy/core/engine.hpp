#ifndef BAPHY_CORE_ENGINE_HPP
#define BAPHY_CORE_ENGINE_HPP

#include "baphy/core/module/application.hpp"
#include "baphy/core/module/window.hpp"
#include "baphy/core/hermes.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/dear_imgui.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/util/platform.hpp"
#include "baphy/util/time.hpp"
#include <atomic>
#include <concepts>
#include <thread>

namespace baphy {
class Engine {
public:
  Engine();

  template<typename T>
    requires std::derived_from<T, Application>
  void run_application(const InitializeParams& initialize_params);

private:
  FrameCounter frame_counter_{};
  std::atomic_bool received_shutdown_{false};

  std::jthread render_thread;

  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};
};

template<typename T>
  requires std::derived_from<T, Application>
void Engine::run_application(const InitializeParams& initialize_params) {
  log_platform();

  module_mgr_->create<Application, T>();
  module_mgr_->create<DearImgui>();
  module_mgr_->create<GfxContext>();
  auto window = module_mgr_->create<Window>();

  Hermes::send_nowait<E_Initialize>(initialize_params);

  // Release OpenGL context in main thread so the render thread can have it
  glfwMakeContextCurrent(nullptr);

  render_thread = std::jthread([&]() {
    glfwMakeContextCurrent(window->handle());

    while (!received_shutdown_) {
      Hermes::send_nowait<E_Update>(frame_counter_.dt());

      Hermes::send_nowait<E_StartFrame>();
      Hermes::send_nowait<E_Draw>();
      Hermes::send_nowait<E_EndFrame>();

      frame_counter_.update();
    }
  });

  while (!received_shutdown_) {
    glfwWaitEventsTimeout(0);
  }

  render_thread.join();

  Hermes::send_nowait_rev<E_Shutdown>();
}
} // namespace baphy

BAPHY_PRAISE_HERMES(baphy::Engine);

#endif//BAPHY_CORE_ENGINE_HPP
