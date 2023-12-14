#ifndef IMPERATOR_CORE_ENGINE_HPP
#define IMPERATOR_CORE_ENGINE_HPP

#include "imperator/core/module/application.hpp"
#include "imperator/core/module/window.hpp"
#include "imperator/core/hermes.hpp"
#include "imperator/core/module_mgr.hpp"
#include "imperator/gfx/module/dear_imgui.hpp"
#include "imperator/gfx/module/gfx_context.hpp"
#include "imperator/util/platform.hpp"
#include "imperator/util/time.hpp"
#include <atomic>
#include <concepts>
#include <thread>

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

  std::jthread render_thread;

  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};

  void r_glfw_set_window_size_(const E_GlfwSetWindowSize& p);
  void r_glfw_set_window_pos_(const E_GlfwSetWindowPos& p);
  void r_glfw_set_window_title_(const E_GlfwSetWindowTitle& p);
  void r_glfw_set_window_icon_(const E_GlfwSetWindowIcon& p);
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

    Hermes::poll<E_GlfwSetWindowSize>(EPI<Engine>::name);
    Hermes::poll<E_GlfwSetWindowPos>(EPI<Engine>::name);
    Hermes::poll<E_GlfwSetWindowTitle>(EPI<Engine>::name);
    Hermes::poll<E_GlfwSetWindowIcon>(EPI<Engine>::name);
  }

  render_thread.join();

  Hermes::send_nowait_rev<E_Shutdown>();
}
} // namespace imp

#endif//IMPERATOR_CORE_ENGINE_HPP
