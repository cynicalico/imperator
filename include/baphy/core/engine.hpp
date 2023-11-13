#ifndef BAPHY_CORE_ENGINE_HPP
#define BAPHY_CORE_ENGINE_HPP

#include "baphy/core/module/application.hpp"
#include "baphy/core/hermes.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/util/time.hpp"
#include <concepts>

namespace baphy {

class Engine {
public:
  Engine();

  template<typename T>
  requires std::derived_from<T, Application>
  void run_application(const InitializeParams &initialize_params);

private:
  FrameCounter frame_counter_{};
  bool received_shutdown_{false};

  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};
};

template<typename T>
requires std::derived_from<T, Application>
void Engine::run_application(const InitializeParams &initialize_params) {
  module_mgr_->create<Application, T>();

  Hermes::send_nowait<E_Initialize>(initialize_params);
  while (!received_shutdown_) {
    Hermes::send_nowait<E_Update>(frame_counter_.dt());

    frame_counter_.update();
  }

  Hermes::send_nowait_rev<E_Shutdown>();
}

} // namespace baphy

BAPHY_PRAY_TO_HERMES(baphy::Engine);

#endif//BAPHY_CORE_ENGINE_HPP
