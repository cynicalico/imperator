#ifndef IMPERATOR_MODULE_APPLICATION_H
#define IMPERATOR_MODULE_APPLICATION_H

#include "imperator/core/engine.h"
#include "imperator/module/window.h"

namespace imp {
class Application : public Module<Application> {
public:
  std::shared_ptr<Window> window;

  explicit Application(std::shared_ptr<Engine> engine);
};
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::Application);

#endif//IMPERATOR_MODULE_APPLICATION_H
