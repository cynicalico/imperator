#ifndef IMPERATOR_CORE_GLFW_WRAP_H
#define IMPERATOR_CORE_GLFW_WRAP_H

#define GLFW_INCLUDE_NONE

#include "imperator/util/enum_bitops.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"
#include <memory>
#include <string>

#include "imperator/util/log.h"

namespace imp {
enum class WindowMode {
    windowed,
    fullscreen,
    borderless
};

enum class WindowFlags {
    none = 1 << 0,
    resizable = 1 << 1,
    hidden = 1 << 2,
    undecorated = 1 << 3,
    centered = 1 << 4
};

struct WindowOpenParams {
    std::string title{"Imperator Window"};

    glm::ivec2 size{1, 1};
    glm::ivec2 pos{0, 0};

    int monitor_num{0};

    WindowMode mode{WindowMode::windowed};
    WindowFlags flags{WindowFlags::none};
};

struct GLFWwindowDeleter {
    void operator()(GLFWwindow *ptr) const {
        glfwDestroyWindow(ptr);
        IMPERATOR_LOG_DEBUG("Window destroyed");
    }
};

using UniqGLFWwindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;
} // namespace imp

ENUM_ENABLE_BITOPS(imp::WindowFlags);

#endif//IMPERATOR_CORE_GLFW_WRAP_H
