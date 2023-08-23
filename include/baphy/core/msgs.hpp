#ifndef BAPHY_CORE_MSGS_HPP
#define BAPHY_CORE_MSGS_HPP

#define GLFW_INCLUDE_NONE
#include "baphy/core/msgs_types.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include "spdlog/common.h"
#include <cstdint>
#include <memory>
#include <string>

// This gets defined when we set the spdlog level, but the intent is for
// it to get defined in the log.hpp header
#undef SPDLOG_ACTIVE_LEVEL

namespace baphy {

struct EInitialize {
  const InitializeParams params;
};

struct EShutdown {};

struct EShutdownEngine {};

struct EStartApplication {};

struct EUpdate {
  double dt;

  double fps;
  std::uint64_t timestamp;
};

struct EStartFrame {};

struct EDraw {};

struct EFlush {
  glm::mat4 projection;
};

struct EEndFrame {};

struct ELogMsg {
  std::string text;
  spdlog::level::level_enum level;
};

struct EDefaultAudioDeviceChanged {
  std::string name;
};

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

#endif//BAPHY_CORE_MSGS_HPP
