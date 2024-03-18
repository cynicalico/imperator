#ifndef IMPERATOR_MODULE_APPLICATION_H
#define IMPERATOR_MODULE_APPLICATION_H

#include "imperator/core/module_mgr.h"
#include "imperator/module/window.h"

namespace imp {
class Application : public Module<Application> {
public:
  std::shared_ptr<EventBus> event_bus{nullptr};
  std::shared_ptr<Window> window{nullptr};

  explicit Application(std::weak_ptr<ModuleMgr> module_mgr);
};
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::Application);

#endif//IMPERATOR_MODULE_APPLICATION_H
