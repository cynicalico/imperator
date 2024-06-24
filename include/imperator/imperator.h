#ifndef IMPERATOR_IMPERATOR_H
#define IMPERATOR_IMPERATOR_H

#include "fmt/chrono.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "fmt/xchar.h"
#include "imperator/core/events.h"
#include "imperator/module/application.h"
#include "imperator/module/audio_mgr.h"
#include "imperator/module/event_bus.h"
#include "imperator/module/gfx/2d/g2d.h"
#include "imperator/module/gfx/gfx_context.h"
#include "imperator/module/input_mgr.h"
#include "imperator/module/module_mgr.h"
#include "imperator/module/timer_mgr.h"
#include "imperator/module/window.h"
#include "imperator/util/averagers.h"
#include "imperator/util/io.h"
#include "imperator/util/log.h"
#include "imperator/util/platform.h"
#include "imperator/util/rnd.h"
#include "imperator/util/time.h"

namespace imp {
struct ApplicationParams {
    WindowOpenParams window;
    GfxParams gfx;
};

template<typename T>
    requires std::derived_from<T, Application>
void mainloop(ApplicationParams params) {
    register_glfw_error_callback();
    if (!glfwInit()) { std::exit(EXIT_FAILURE); }

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    IMPERATOR_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, rev);

    auto module_mgr = ModuleMgr();

    { /* scope to destruct module refs */
        const auto event_bus = module_mgr.create<EventBus>();
        internal::set_global_callback_user_pointer(event_bus.get());

        const auto window = module_mgr.create<Window>(params.window, params.gfx.backend_version);
        module_mgr.create<GfxContext>(params.gfx);

        module_mgr.create<AudioMgr>();
        module_mgr.create<InputMgr>();
        module_mgr.create<TimerMgr>();

        module_mgr.create<Application, T>();

        auto t = Ticker();
        while (!window->should_close()) {
            event_bus->send_nowait<E_Update>(t.dt_sec());

            event_bus->send_nowait<E_StartFrame>();
            event_bus->send_nowait<E_Draw>();
            event_bus->send_nowait<E_EndFrame>();

            glfwPollEvents();
            t.tick();
        }

        internal::clear_global_callback_user_pointer();
    }
    module_mgr.clear();

    glfwTerminate();
    IMPERATOR_LOG_DEBUG("Terminated GLFW");
}
} // namespace imp

#endif //IMPERATOR_IMPERATOR_H
