#include "imperator/module/window.h"

#include "imperator/core/glfw_callbacks.h"
#include "imperator/util/platform.h"

namespace imp {
Window::Window(ModuleMgr &module_mgr, WindowOpenParams open_params, glm::ivec2 backend_version)
        : Module(module_mgr) {
    event_bus = module_mgr_.get<EventBus>();

    event_bus->sub<E_Update>(module_name_, [&](const auto &p) { r_update_(p); });

    event_bus->sub<E_EndFrame>(module_name_, [&](const auto &p) { r_end_frame_(p); });

    event_bus->sub<E_GlfwWindowClose>(module_name_, [&](const auto &p) { r_glfw_window_close_(p); });
    event_bus->sub<E_GlfwWindowSize>(module_name_, [&](const auto &p) { r_glfw_window_size_(p); });
    event_bus->sub<E_GlfwFramebufferSize>(module_name_, [&](const auto &p) { r_glfw_framebuffer_size_(p); });
    event_bus->sub<E_GlfwWindowContentScale>(module_name_, [&](const auto &p) { r_glfw_window_content_scale_(p); });
    event_bus->sub<E_GlfwWindowPos>(module_name_, [&](const auto &p) { r_glfw_window_pos_(p); });
    event_bus->sub<E_GlfwWindowIconify>(module_name_, [&](const auto &p) { r_glfw_window_iconify_(p); });
    event_bus->sub<E_GlfwWindowMaximize>(module_name_, [&](const auto &p) { r_glfw_window_maximize_(p); });
    event_bus->sub<E_GlfwWindowFocus>(module_name_, [&](const auto &p) { r_glfw_window_focus_(p); });
    event_bus->sub<E_GlfwWindowRefresh>(module_name_, [&](const auto &p) { r_glfw_window_refresh_(p); });
    event_bus->sub<E_GlfwMonitor>(module_name_, [&](const auto &p) { r_glfw_monitor_(p); });

    open_(open_params, backend_version);
}

Window::~Window() { clear_glfw_user_pointer(handle()); }

bool Window::should_close() const { return glfwWindowShouldClose(handle()) == GLFW_TRUE; }

void Window::set_should_close(bool v) { glfwSetWindowShouldClose(handle(), v ? GLFW_TRUE : GLFW_FALSE); }

GLFWmonitor *Window::get_monitor_(int monitor_num) {
    int monitor_count = 0;
    const auto monitors = glfwGetMonitors(&monitor_count);

    if (monitor_num >= monitor_count) {
        IMPERATOR_LOG_WARN(
                "Monitor {} out of range ({} available); defaulting to primary",
                monitor_num,
                monitor_count
        );
        return monitors[0];
    }
    return monitors[monitor_num];
}

void Window::open_(WindowOpenParams open_params, glm::ivec2 backend_version) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, backend_version.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, backend_version.y);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#if !defined(NDEBUG)
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (open_params.mode == WindowMode::fullscreen || open_params.mode == WindowMode::borderless)
        open_fullscreen_(
                open_params
        );
    else open_windowed_(open_params);

    set_glfw_user_pointer(handle(), event_bus.get());
    register_glfw_callbacks(handle());

    mode_ = open_params.mode;
    title_ = open_params.title;

    // This isn't sent at the beginning of the program, so we send it first
    // Sent as an immediate event in case we ever do anything special when
    // the window size is changed
    if (open_params.mode == WindowMode::windowed) {
        event_bus->send_nowait<E_GlfwWindowSize>(handle(), open_params.size.x, open_params.size.y);
    } else {
        GLFWmonitor *monitor = get_monitor_(open_params.monitor_num);
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        event_bus->send_nowait<E_GlfwWindowSize>(handle(), mode->width, mode->height);
    }

    if (open_params.mode == WindowMode::windowed && !is_flag_set(open_params.flags, WindowFlags::hidden))
        glfwShowWindow(
                handle()
        );
}

void Window::open_fullscreen_(WindowOpenParams open_params) {
#if defined(IMPERATOR_PLATFORM_WINDOWS)
    GLFWmonitor* monitor = get_monitor_(open_params.monitor_num);
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* h;
    if (open_params.mode == WindowMode::borderless) {
      // borderless = true;

      glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
      // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
      // This seems to cause flickering
      glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
      h = glfwCreateWindow(mode->width, mode->height, open_params.title.c_str(), nullptr, nullptr);
    } else
      h = glfwCreateWindow(mode->width, mode->height, open_params.title.c_str(), monitor, nullptr);

    if (!h) {
      const char* description;
      int code = glfwGetError(&description);
      IMPERATOR_LOG_CRITICAL("Failed to create GLFW window: ({}) {}", code, description);
      glfwTerminate();
      std::exit(EXIT_FAILURE);
    }

    glfw_window_ = UniqGLFWwindow(h);
    IMPERATOR_LOG_DEBUG("Created GLFW window");

    if (open_params.mode == WindowMode::borderless) {
      int base_x, base_y;
      glfwGetMonitorPos(monitor, &base_x, &base_y);
      glfwSetWindowPos(handle(), base_x, base_y);
    }
#elif defined(IMPERATOR_PLATFORM_LINUX)
    /* We are making the assumption that the user is running a version of X11
     * that treats *all* fullscreen windows as borderless fullscreen windows.
     * This seems to generally be true for a good majority of systems. This may
     * also just act exactly like a normal fullscreen, there's not really any
     * way to tell ahead of time.
     */

    GLFWmonitor *monitor = get_monitor_(open_params.monitor_num);
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Why is this necessary?
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    if (open_params.mode == WindowMode::borderless) {
        //    borderless = true;

        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    }

    auto h = glfwCreateWindow(mode->width, mode->height, open_params.title.c_str(), monitor, nullptr);
    if (!h) {
        const char *description;
        int code = glfwGetError(&description);
        IMPERATOR_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfw_window_ = UniqGLFWwindow(h);
    IMPERATOR_LOG_DEBUG("Created GLFW window");
#endif
}

void Window::open_windowed_(WindowOpenParams open_params) {
    GLFWmonitor *monitor = get_monitor_(open_params.monitor_num);
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(
            GLFW_RESIZABLE,
            is_flag_set(open_params.flags, WindowFlags::resizable)
            ? GLFW_TRUE
            : GLFW_FALSE
    );
    glfwWindowHint(
            GLFW_DECORATED,
            is_flag_set(open_params.flags, WindowFlags::undecorated)
            ? GLFW_FALSE
            : GLFW_TRUE
    );

    auto h = glfwCreateWindow(
            open_params.size.x,
            open_params.size.y,
            open_params.title.c_str(),
            nullptr,
            nullptr
    );
    if (!h) {
        const char *description;
        int code = glfwGetError(&description);
        IMPERATOR_LOG_CRITICAL("Failed to create GLFW window:\n* ({}) {}", code, description);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfw_window_ = UniqGLFWwindow(h);
    IMPERATOR_LOG_DEBUG("Created GLFW window");

    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    if (is_flag_set(open_params.flags, WindowFlags::centered))
        glfwSetWindowPos(
                handle(),
                base_x + (mode->width - open_params.size.x) / 2,
                base_y + (mode->height - open_params.size.y) / 2
        );
    else
        glfwSetWindowPos(
                handle(),
                base_x + open_params.pos.x,
                base_y + open_params.pos.y
        );
}

void Window::r_update_(const E_Update &p) {
    event_bus->poll<E_GlfwWindowClose>(module_name_);
    event_bus->poll<E_GlfwWindowSize>(module_name_);
    event_bus->poll<E_GlfwFramebufferSize>(module_name_);
    event_bus->poll<E_GlfwWindowContentScale>(module_name_);
    event_bus->poll<E_GlfwWindowPos>(module_name_);
    event_bus->poll<E_GlfwWindowIconify>(module_name_);
    event_bus->poll<E_GlfwWindowMaximize>(module_name_);
    event_bus->poll<E_GlfwWindowFocus>(module_name_);
    event_bus->poll<E_GlfwWindowRefresh>(module_name_);
    event_bus->poll<E_GlfwMonitor>(module_name_);
}

void Window::r_end_frame_(const E_EndFrame &p) { glfwSwapBuffers(handle()); }

void Window::r_glfw_window_close_(const E_GlfwWindowClose &p) {}

void Window::r_glfw_window_size_(const E_GlfwWindowSize &p) {
    size_.x = p.width;
    size_.y = p.height;
}

void Window::r_glfw_framebuffer_size_(const E_GlfwFramebufferSize &p) {}

void Window::r_glfw_window_content_scale_(const E_GlfwWindowContentScale &p) {}

void Window::r_glfw_window_pos_(const E_GlfwWindowPos &p) {
    pos_.x = p.xpos;
    pos_.y = p.ypos;
}

void Window::r_glfw_window_iconify_(const E_GlfwWindowIconify &p) {}

void Window::r_glfw_window_maximize_(const E_GlfwWindowMaximize &p) {}

void Window::r_glfw_window_focus_(const E_GlfwWindowFocus &p) {}

void Window::r_glfw_window_refresh_(const E_GlfwWindowRefresh &p) {}

void Window::r_glfw_monitor_(const E_GlfwMonitor &p) {}
} // namespace imp
