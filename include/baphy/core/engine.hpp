#ifndef BAPHY_CORE_ENGINE_HPP
#define BAPHY_CORE_ENGINE_HPP

#include "baphy/core/module/application.hpp"
#include "baphy/core/module/module.hpp"
#include "baphy/core/module/window.hpp"
#include <memory>

namespace baphy {

class Engine {
public:
  Engine();

  template<typename T>
  requires std::derived_from<T, Application>
  void run_application();

private:
  std::shared_ptr<ModuleMgr> module_mgr_{nullptr};
};

template<typename T>
requires std::derived_from<T, Application>
void Engine::run_application() {
  module_mgr_->create<Window>();
  module_mgr_->create<Application, T>();

  EventBus::send_nowait<EInitialize>();

  EventBus::send_nowait<EShutdown>();
}

} // namespace baphy

#endif //BAPHY_CORE_ENGINE_HPP
