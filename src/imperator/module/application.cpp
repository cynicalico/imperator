#include "imperator/module/application.h"

namespace imp {
Application::Application(std::shared_ptr<Engine> engine)
  : Module<Application>(std::move(engine)) {
  window = imp_engine->get_module<Window>();
}
} // namespace imp
