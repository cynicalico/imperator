#include "myco/core/modules/application.h"

namespace myco {

void Application::initialize_(const myco::Initialize &e) {
    Module::initialize_(e);
}

} // namespace myco
