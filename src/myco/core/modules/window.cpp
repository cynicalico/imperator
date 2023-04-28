#include "myco/core/modules/window.h"

namespace myco {

std::once_flag Window::initialized_glfw_;

void Window::open(const myco::WindowOpenParams &params) {
    open_params_ = params;
}

WindowOpenParams Window::get_open_params() const {
    return open_params_;
}

void Window::initialize_(const myco::Initialize &e) {
    Module::initialize_(e);

    std::call_once(initialized_glfw_, []() {
        if (glfwInit() == GLFW_FALSE)
            std::exit(EXIT_FAILURE);

        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        MYCO_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, revision);
    });
}

} // namespace myco
