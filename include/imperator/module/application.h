#ifndef IMPERATOR_MODULE_APPLICATION_H
#define IMPERATOR_MODULE_APPLICATION_H

#include "imperator/module/audio_mgr.h"
#include "imperator/module/gfx/gfx_context.h"
#include "imperator/module/input_mgr.h"
#include "imperator/module/module_mgr.h"
#include "imperator/module/timer_mgr.h"
#include "imperator/module/window.h"

namespace imp {
class Application : public Module<Application> {
public:
    std::shared_ptr<AudioMgr> audio{nullptr};
    std::shared_ptr<GfxContext> ctx{nullptr};
    std::shared_ptr<EventBus> event_bus{nullptr};
    std::shared_ptr<InputMgr> inputs{nullptr};
    std::shared_ptr<TimerMgr> timers{nullptr};
    std::shared_ptr<Window> window{nullptr};

    explicit Application(ModuleMgr &module_mgr);

protected:
    virtual void update(double dt);

    virtual void draw();

private:
    void r_update_(const E_Update &p);

    void r_draw_(const E_Draw &p);
};
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::Application);

#endif //IMPERATOR_MODULE_APPLICATION_H
