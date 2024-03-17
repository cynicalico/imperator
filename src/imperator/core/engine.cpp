#include "imperator/core/engine.h"

#include "imperator/module/application.h"
#include "imperator/module/window.h"
#include "imperator/util/platform.h"

namespace imp {
std::shared_ptr<Engine> Engine::create() {
  std::shared_ptr<Engine> e = std::make_shared<Engine>();

  log_platform();
  e->event_bus_ = std::make_shared<EventBus>();

  return e;
}

void Engine::mainloop_(WindowOpenParams params, const std::function<void()>& application_creation_function) {
  auto window = create_module<Window>(params);
  application_creation_function();

  while (!window->should_close()) {
    send_event_nowait<E_Update>();

    glfwPollEvents();
  }

  send_event_nowait<E_FreeEngine>();
  modules_.clear();
}
} // namespace imp
