#include "imperator/module/application.h"

namespace imp {
Application::Application(ModuleMgr &module_mgr) : Module(module_mgr) {
    audio = module_mgr_.get<AudioMgr>();
    ctx = module_mgr_.get<GfxContext>();
    event_bus = module_mgr_.get<EventBus>();
    inputs = module_mgr_.get<InputMgr>();
    timers = module_mgr_.get<TimerMgr>();
    window = module_mgr_.get<Window>();

    event_bus->sub<E_Update>(
            module_name_,
            {ModuleInfo<AudioMgr>::name, ModuleInfo<InputMgr>::name},
            [&](const auto &p) { r_update_(p); }
    );
    event_bus->sub<E_Draw>(module_name_, [&](const auto &p) { r_draw_(p); });
}

void Application::update(double dt) {}

void Application::draw() {}

void Application::r_update_(const E_Update &p) { update(p.dt); }

void Application::r_draw_(const E_Draw &) { draw(); }
} // namespace imp
