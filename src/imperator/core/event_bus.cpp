#include "imperator/core/event_bus.h"

namespace imp {
EventBus::EventBus(std::weak_ptr<ModuleMgr> module_mgr)
  : Module(std::move(module_mgr)) {}
} // namespace imp
