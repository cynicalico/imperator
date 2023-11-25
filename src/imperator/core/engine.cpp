#include "imperator/core/engine.hpp"

namespace imp {
Engine::Engine() {
  module_mgr_ = std::make_shared<ModuleMgr>();

  Hermes::sub<E_ShutdownEngine>(EPI<Engine>::name, [&](const auto&) {
    received_shutdown_ = true;
  });
}
} // namespace imp
