#include "imperator/module/application.h"

namespace imp {
Application::Application(std::weak_ptr<ModuleMgr> module_mgr)
  : Module(std::move(module_mgr)) {
  event_bus = module_mgr_.lock()->get<EventBus>();
  window = module_mgr_.lock()->get<Window>();
}
} // namespace imp
