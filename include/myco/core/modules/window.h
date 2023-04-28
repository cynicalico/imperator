#ifndef MYCO_CORE_MODULES_WINDOW_H
#define MYCO_CORE_MODULES_WINDOW_H

#define GLFW_INCLUDE_NONE
#include "myco/core/module.h"
#include "myco/util/enum_bitops.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

#include <string>

namespace myco {

enum class WindowFlags {
    none        = 1 << 0,
    vsync       = 1 << 1,
    resizable   = 1 << 2,
    hidden      = 1 << 3,
    centered    = 1 << 4,
    fullscreen  = 1 << 5,
    borderless  = 1 << 6,
    undecorated = 1 << 7,
};

struct WindowOpenParams {
    std::string title{"Myco Window"};

    glm::ivec2 size{0, 0};
    glm::ivec2 pos{0, 0};

    int monitor_num{0};

    WindowFlags flags{WindowFlags::none};

    glm::ivec2 backend_version{3, 3};

    bool win32_force_light_mode{false};
    bool win32_force_dark_mode{false};
};

class Window : public Module<Window> {
public:
    Window() : Module<Window>() {}

    ~Window() override = default;

    void open(const WindowOpenParams &params);
    WindowOpenParams get_open_params() const;

private:
    static std::once_flag initialized_glfw_;

    WindowOpenParams open_params_{};

    void initialize_(const Initialize &e) override;
};

} // namespace myco

MYCO_DECLARE_MODULE(myco::Window);
ENABLE_BITOPS(myco::WindowFlags);

#endif//MYCO_CORE_MODULES_WINDOW_H
