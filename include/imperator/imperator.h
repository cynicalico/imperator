#ifndef IMPERATOR_IMPERATOR_H
#define IMPERATOR_IMPERATOR_H

#include "imperator/core/events.h"
#include "imperator/module/gfx/gfx_context.h"
#include "imperator/module/application.h"
#include "imperator/module/event_bus.h"
#include "imperator/module/module_mgr.h"
#include "imperator/module/window.h"

#include "imperator/util/averagers.h"
#include "imperator/util/io.h"
#include "imperator/util/log.h"
#include "imperator/util/platform.h"
#include "imperator/util/rnd.h"
#include "imperator/util/time.h"

#include "fmt/chrono.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace imp {
struct ApplicationParams {
  WindowOpenParams window;
  GfxParams gfx;
};

template <typename T>
  requires std::derived_from<T, Application>
void mainloop(ApplicationParams params) {
  register_glfw_error_callback();
  if (!glfwInit())
    std::exit(EXIT_FAILURE);

  int major, minor, rev;
  glfwGetVersion(&major, &minor, &rev);
  IMPERATOR_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, rev);

  auto module_mgr = ModuleMgr();
  { /* scope to destruct module refs */
    const auto event_bus = module_mgr.create<EventBus>();
    set_global_user_pointer(event_bus.get());

    const auto window = module_mgr.create<Window>(
      params.window,
      params.gfx.backend_version
    );
    module_mgr.create<GfxContext>(params.gfx);

    module_mgr.create<Application, T>();

    while (!window->should_close()) {
      event_bus->send_nowait<E_Update>(0.0);

      event_bus->send_nowait<E_StartFrame>();
      event_bus->send_nowait<E_Draw>();
      event_bus->send_nowait<E_EndFrame>();

      glfwPollEvents();
    }

    clear_global_user_pointer();
  }
  module_mgr.clear();

  glfwTerminate();
  IMPERATOR_LOG_DEBUG("Terminated GLFW");
}
} // namespace imp

#endif//IMPERATOR_IMPERATOR_H
