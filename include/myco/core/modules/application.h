#ifndef MYCO_CORE_MODULES_APPLICATION_H
#define MYCO_CORE_MODULES_APPLICATION_H

#include "myco/core/module.h"
#include "myco/core/modules/window.h"

namespace myco {

class Application : public Module<Application> {
public:
    std::shared_ptr<Window> window{nullptr};

    Application() : Module<Application>({
        ModuleInfo<Window>::name
    }) {}

    ~Application() override = default;

protected:
    virtual void initialize();
    virtual void update(double dt);
    virtual void draw();



private:
    void initialize_(const Initialize &e) override;
    void start_(const StartApplication &e);

    void start_frame_();
    void end_frame_();
};

} // namespace myco

MYCO_DECLARE_MODULE(myco::Application);

#endif//MYCO_CORE_MODULES_APPLICATION_H
