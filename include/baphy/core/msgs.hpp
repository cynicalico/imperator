#ifndef BAPHY_CORE_MSGS_HPP
#define BAPHY_CORE_MSGS_HPP

#define GLFW_INCLUDE_NONE
#include "baphy/core/msgs_types.hpp"
#include "GLFW/glfw3.h"
#include <memory>

namespace baphy {

struct EInitialize {
  const WindowOpenParams window_open_params;
};

struct EShutdown {};

struct EStartApplication {};

struct EUpdate {
  double dt;
};

struct EStartFrame {};

struct EDraw {};

struct EEndFrame {};

/* GLFW CALLBACKS */

struct EGlfwWindowClose {
  GLFWwindow *window;
};

struct EGlfwWindowSize {
  GLFWwindow *window;
  int width;
  int height;
};

struct EGlfwFramebufferSize {
  GLFWwindow *window;
  int width;
  int height;
};

struct EGlfwWindowContentScale {
  GLFWwindow *window;
  float xscale;
  float yscale;
};

struct EGlfwWindowPos {
  GLFWwindow *window;
  int xpos;
  int ypos;
};

struct EGlfwWindowIconify {
  GLFWwindow *window;
  int iconified;
};

struct EGlfwWindowMaximize {
  GLFWwindow *window;
  int maximized;
};

struct EGlfwWindowFocus {
  GLFWwindow *window;
  int focused;
};

struct EGlfwWindowRefresh {
  GLFWwindow *window;
};

struct EGlfwMonitor {
  GLFWmonitor *monitor;
  int event;
};

struct EGlfwKey {
  GLFWwindow *window;
  int key;
  int scancode;
  int action;
  int mods;
};

struct EGlfwCharacter {
  GLFWwindow *window;
  unsigned int codepoint;
};

struct EGlfwCursorPos {
  GLFWwindow *window;
  double xpos;
  double ypos;
};

struct EGlfwCursorEnter {
  GLFWwindow *window;
  int entered;
};

struct EGlfwMouseButton {
  GLFWwindow *window;
  int button;
  int action;
  int mods;
};

struct EGlfwScroll {
  GLFWwindow *window;
  double xoffset;
  double yoffset;
};

struct EGlfwJoystick {
  int jid;
  int event;
};

struct EGlfwDrop {
  GLFWwindow *window;
  int count;
  const char **paths;
};

} // namespace baphy

#endif //BAPHY_CORE_MSGS_HPP
