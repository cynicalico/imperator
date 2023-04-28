#ifndef MYCO_CORE_MSGS_H
#define MYCO_CORE_MSGS_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <memory>

namespace myco {

class Engine;

struct Initialize {
    const std::shared_ptr<Engine> &engine;
};

struct Shutdown {};

struct ReleaseEngine {};

/* GLFW CALLBACKS */

struct GlfwWindowClose {
    GLFWwindow *window;
};

struct GlfwWindowSize {
    GLFWwindow *window;
    int width;
    int height;
};

struct GlfwFramebufferSize {
    GLFWwindow *window;
    int width;
    int height;
};

struct GlfwWindowContentScale {
    GLFWwindow *window;
    float xscale;
    float yscale;
};

struct GlfwWindowPos {
    GLFWwindow *window;
    int xpos;
    int ypos;
};

struct GlfwWindowIconify {
    GLFWwindow *window;
    int iconified;
};

struct GlfwWindowMaximize {
    GLFWwindow *window;
    int maximized;
};

struct GlfwWindowFocus {
    GLFWwindow *window;
    int focused;
};

struct GlfwWindowRefresh {
    GLFWwindow *window;
};

struct GlfwMonitor {
    GLFWmonitor *monitor;
    int event;
};

struct GlfwKey {
    GLFWwindow *window;
    int key;
    int scancode;
    int action;
    int mods;
};

struct GlfwCharacter {
    GLFWwindow *window;
    unsigned int codepoint;
};

struct GlfwCursorPos {
    GLFWwindow *window;
    double xpos;
    double ypos;
};

struct GlfwCursorEnter {
    GLFWwindow *window;
    int entered;
};

struct GlfwMouseButton {
    GLFWwindow *window;
    int button;
    int action;
    int mods;
};

struct GlfwScroll {
    GLFWwindow *window;
    double xoffset;
    double yoffset;
};

struct GlfwJoystick {
    int jid;
    int event;
};

struct GlfwDrop {
    GLFWwindow *window;
    int count;
    const char **paths;
};

} // namespace myco

#endif//MYCO_CORE_MSGS_H
