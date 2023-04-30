#include "myco/core/modules/application.h"

#include "myco/core/engine.h"
#include "myco/util/log.h"

namespace myco {

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::initialize_(const myco::Initialize &e) {
    Module::initialize_(e);

    EventBus::sub<StartApplication>(name, [&](const auto &p){ start_(p); });

    EventBus::sub<Update>(
            name,
            {
//                    ModuleInfo<InputMgr>::name,
//                    ModuleInfo<TimerMgr>::name,
                    ModuleInfo<Window>::name,
            },
            [&](const auto &e){ update(e.dt); }
    );

    EventBus::sub<StartFrame>(name, [&](const auto &){ start_frame_(); });
    EventBus::sub<Draw>(name, [&](const auto &){ draw(); });
    EventBus::sub<EndFrame>(name, [&](const auto &){ end_frame_(); });

    window = engine->get_module<Window>();
}

void Application::start_(const StartApplication &e) {
    window->open(e.window_open_params);
//    ctx = std::make_shared<myco::Context2D>(*window);
//    dear = std::make_unique<myco::Dear>(*window, *ctx);

    initialize();
}

void Application::start_frame_() {
//    if (dear)
//        dear->new_frame();
}

void Application::end_frame_() {
//    draw_stickies_();
//
//    if (debug_overlay_.active)
//        draw_debug_overlay_();
//
//    if (dear)
//        dear->render();
}

} // namespace myco
