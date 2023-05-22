#ifndef MYCO_CORE_MODULES_WINDOW_H
#define MYCO_CORE_MODULES_WINDOW_H

#define GLFW_INCLUDE_NONE
#include "myco/core/module.h"
#include "myco/util/enum_bitops.h"
#include "myco/util/platform.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

#include <filesystem>
#include <string>

#if defined(MYCO_PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

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
    GLFWwindow *glfw_handle{nullptr};

    int x{0}, y{0};
    int w{0}, h{0};
    bool decorated{true};
    bool borderless{false};
    bool resizable{false};
    bool vsync{false};

    Window() : Module<Window>() {}

    ~Window() override = default;

    void open(const WindowOpenParams &params);
    WindowOpenParams get_open_params() const;

    void set_icon_dir(const std::filesystem::path &dir);
    void set_icon(const std::vector<std::filesystem::path> &paths);
    void set_icon(const std::filesystem::path &path);

    void set_x(int xpos);
    void set_y(int ypos);

    void set_w(int width);
    void set_h(int height);

    void set_decorated(bool is_decorated);
    void set_resizable(bool is_resizable);
    void set_vsync(bool is_enabled);

    bool should_close() const;
    void set_should_close(bool should);

    void swap() const;

private:
    static std::once_flag initialized_glfw_;

    WindowOpenParams open_params_{};

    void initialize_(const Initialize &e) override;
    void update_(double dt);
    void start_frame_(const StartFrame &e);
    void end_frame_(const EndFrame &e);

    void close_callback_(const GlfwWindowClose &e);

    GLFWmonitor *get_monitor_(int monitor_num);
    void open_fullscreen_(const WindowOpenParams &params);
    void open_windowed_(const WindowOpenParams &params);

#if defined(MYCO_PLATFORM_WINDOWS)
    HWND win32_hwnd_{nullptr};
    WNDPROC win32_saved_WndProc_{nullptr};
    bool win32_force_light_mode_{false};
    bool win32_force_dark_mode_{false};

    static void set_win32_titlebar_color_(HWND hwnd);
    static LRESULT CALLBACK WndProc_(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

} // namespace myco

MYCO_DECLARE_MODULE(myco::Window);
ENABLE_BITOPS(myco::WindowFlags);

#endif//MYCO_CORE_MODULES_WINDOW_H