#include "imperator/module/gfx/gfx_context.h"

#if defined(IMPERATOR_PLATFORM_WINDOWS)
#elif defined(IMPERATOR_PLATFORM_LINUX)
#    define GLFW_EXPOSE_NATIVE_X11
#    define GLFW_EXPOSE_NATIVE_GLX
#    define GLFW_NATIVE_INCLUDE_NONE

#    include "glad/glx.h"

#    include <X11/Xlib.h>
#    include <X11/extensions/Xrandr.h>

// #define GLFW_EXPOSE_NATIVE_WAYLAND
// #define GLFW_EXPOSE_NATIVE_EGL
// #define GLFW_NATIVE_INCLUDE_NONE
// #include "glad/egl.h"

#    include "GLFW/glfw3native.h"

#endif

namespace imp {
GfxContext::GfxContext(ModuleMgr &module_mgr, GfxParams params) : Module(module_mgr), gl(GladGLContext()) {
    window = module_mgr_.get<Window>();

    glfwMakeContextCurrent(window->handle());

    auto glad_version = gladLoadGLContext(&gl, glfwGetProcAddress);
    if (glad_version == 0) {
        IMPERATOR_LOG_CRITICAL("Failed to initialize OpenGL");
        std::exit(EXIT_FAILURE);
    }

    glm::ivec2 version;
    version.x = GLAD_VERSION_MAJOR(glad_version);
    version.y = GLAD_VERSION_MINOR(glad_version);
    IMPERATOR_LOG_DEBUG("Initialized OpenGL");
    IMPERATOR_LOG_DEBUG("=> Version: {}", (char *)gl.GetString(GL_VERSION));
    IMPERATOR_LOG_DEBUG("=> Vendor: {}", (char *)gl.GetString(GL_VENDOR));
    IMPERATOR_LOG_DEBUG("=> Renderer: {}", (char *)gl.GetString(GL_RENDERER));

    if (version.x != params.backend_version.x || version.y != params.backend_version.y) {
        IMPERATOR_LOG_WARN("Requested OpenGL v{}.{}", params.backend_version.x, params.backend_version.y);
    }

#if !defined(NDEBUG)
    gl.Enable(GL_DEBUG_OUTPUT);
    gl.DebugMessageCallback(gl_message_callback_, nullptr);
#endif

    initialize_platform_extensions_();

    // We set to false and then the requested value because wglGetSwapInterval doesn't
    // seem to return the correct value on an initial call unless we have
    // explicitly set something beforehand
    set_vsync(false);
    set_vsync(params.vsync);
}

bool GfxContext::is_vsync() const {
    if (use_platform_extensions_) { return platform_is_vsync_(); }
    return false;
}

void GfxContext::set_vsync(bool v) {
    if (use_platform_extensions_) {
        platform_set_vsync_(v);
    } else {
        glfwSwapInterval(v ? 1 : 0);
    }
}

#if defined(IMPERATOR_PLATFORM_WINDOWS)
void GfxContext::initialize_platform_extensions_() {
    // TODO
}

bool GfxContext::platform_is_vsync_() const {
    return false; // TODO
}

void GfxContext::platform_set_vsync_(bool v) {
    // TODO
}
#elif defined(IMPERATOR_PLATFORM_LINUX)

void GfxContext::initialize_platform_extensions_() {
    Display *dpy = glfwGetX11Display();
    int screen = DefaultScreen(dpy);
    auto glx_version = gladLoadGLX(dpy, screen, glfwGetProcAddress);
    if (glx_version == 0) {
        IMPERATOR_LOG_WARN("Failed to initialize GLX");
        return;
    }

    auto version_major = GLAD_VERSION_MAJOR(glx_version);
    auto version_minor = GLAD_VERSION_MINOR(glx_version);
    IMPERATOR_LOG_DEBUG("Initialized GLX v{}.{}", version_major, version_minor);

    // auto display = glfwGetEGLDisplay();
    // auto egl_version = gladLoadEGL(display, glfwGetProcAddress);
    // if (egl_version == 0) {
    //   IMPERATOR_LOG_WARN("Failed to initialize EGL");
    //   return;
    // }
    //
    // auto version_major = GLAD_VERSION_MAJOR(egl_version);
    // auto version_minor = GLAD_VERSION_MINOR(egl_version);
    // IMPERATOR_LOG_DEBUG("Initialized EGL v{}.{}", version_major, version_minor);

    use_platform_extensions_ = true;
}

bool GfxContext::platform_is_vsync_() const {
    Display *dpy = glXGetCurrentDisplay();
    GLXDrawable drawable = glXGetCurrentDrawable();

    unsigned int swap;
    glXQueryDrawable(dpy, drawable, GLX_SWAP_INTERVAL_EXT, &swap);

    return swap > 0;
}

void GfxContext::platform_set_vsync_(bool v) {
    Display *dpy = glXGetCurrentDisplay();
    GLXDrawable drawable = glXGetCurrentDrawable();

    glXSwapIntervalEXT(dpy, drawable, v ? 1 : 0);
}

#endif

void GfxContext::gl_message_callback_(
        GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
        const void *userParam
) {
#define STRINGIFY(e) \
    case e: return #e;
    const std::string source_str = std::invoke([source] {
        switch (source) {
            STRINGIFY(GL_DEBUG_SOURCE_API)
            STRINGIFY(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
            STRINGIFY(GL_DEBUG_SOURCE_SHADER_COMPILER)
            STRINGIFY(GL_DEBUG_SOURCE_THIRD_PARTY)
            STRINGIFY(GL_DEBUG_SOURCE_APPLICATION)
            STRINGIFY(GL_DEBUG_SOURCE_OTHER)
        default: return "?";
        }
    });

    const std::string type_str = std::invoke([type] {
        switch (type) {
            STRINGIFY(GL_DEBUG_TYPE_ERROR)
            STRINGIFY(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
            STRINGIFY(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
            STRINGIFY(GL_DEBUG_TYPE_PORTABILITY)
            STRINGIFY(GL_DEBUG_TYPE_PERFORMANCE)
            STRINGIFY(GL_DEBUG_TYPE_MARKER)
            STRINGIFY(GL_DEBUG_TYPE_PUSH_GROUP)
            STRINGIFY(GL_DEBUG_TYPE_POP_GROUP)
            STRINGIFY(GL_DEBUG_TYPE_OTHER)
        default: return "?";
        }
    });

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        IMPERATOR_LOG_ERROR(
                "OpenGL: source={} type={} id={} msg={}",
                source_str.substr(9),
                type_str.substr(9),
                id,
                message
        );
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        IMPERATOR_LOG_WARN(
                "OpenGL: source={} type={} id={} msg={}",
                source_str.substr(9),
                type_str.substr(9),
                id,
                message
        );
        break;
    case GL_DEBUG_SEVERITY_LOW:
        IMPERATOR_LOG_DEBUG(
                "OpenGL: source={} type={} id={} msg={}",
                source_str.substr(9),
                type_str.substr(9),
                id,
                message
        );
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        IMPERATOR_LOG_TRACE(
                "OpenGL: source={} type={} id={} msg={}",
                source_str.substr(9),
                type_str.substr(9),
                id,
                message
        );
        break;
    default: break; // won't happen
    }
#undef STRINGIFY
}
} // namespace imp
