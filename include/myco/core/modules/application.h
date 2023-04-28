#ifndef MYCO_CORE_MODULES_APPLICATION_H
#define MYCO_CORE_MODULES_APPLICATION_H

#include "myco/core/module.h"
#include "myco/core/modules/window.h"

namespace myco {

class Application : public Module<Application> {
public:
    Application() : Module<Application>({
        ModuleInfo<Window>::name
    }) {}

private:
    void initialize_(const Initialize &e) override;
};

} // namespace myco

MYCO_DECLARE_MODULE(myco::Application);

#endif//MYCO_CORE_MODULES_APPLICATION_H
