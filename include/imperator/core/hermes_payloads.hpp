#ifndef IMPERATOR_CORE_HERMES_PAYLOADS_HPP
#define IMPERATOR_CORE_HERMES_PAYLOADS_HPP

#define GLFW_INCLUDE_NONE
#include "hermes_payloads_types.hpp"
#include "GLFW/glfw3.h"
#include "spdlog/common.h"
#include <filesystem>
#include <string>

// This gets defined when we include spdlog/common.hpp, but the
// intent is for it to get defined in the log.hpp header (or by
// the user)
#undef SPDLOG_ACTIVE_LEVEL

namespace imp {
/* EVENTS */

struct E_Initialize {
  const InitializeParams params;
};

struct E_ShutdownEngine {};

struct E_Shutdown {};

struct E_LogMsg {
  std::string text;
  spdlog::level::level_enum level;
};

struct E_StartFrame {};

struct E_Draw {};

struct E_EndFrame {};

struct E_Update {
  double dt;
};

struct E_GlfwWindowClose {
  GLFWwindow* window;
};

struct E_GlfwWindowSize {
  GLFWwindow* window;
  int width;
  int height;
};

struct E_GlfwFramebufferSize {
  GLFWwindow* window;
  int width;
  int height;
};

struct E_GlfwWindowContentScale {
  GLFWwindow* window;
  float xscale;
  float yscale;
};

struct E_GlfwWindowPos {
  GLFWwindow* window;
  int xpos;
  int ypos;
};

struct E_GlfwWindowIconify {
  GLFWwindow* window;
  int iconified;
};

struct E_GlfwWindowMaximize {
  GLFWwindow* window;
  int maximized;
};

struct E_GlfwWindowFocus {
  GLFWwindow* window;
  int focused;
};

struct E_GlfwWindowRefresh {
  GLFWwindow* window;
};

struct E_GlfwMonitor {
  GLFWmonitor* monitor;
  int event;
};

struct E_GlfwKey {
  GLFWwindow* window;
  int key;
  int scancode;
  int action;
  int mods;
};

struct E_GlfwCharacter {
  GLFWwindow* window;
  unsigned int codepoint;
};

struct E_GlfwCursorPos {
  GLFWwindow* window;
  double xpos;
  double ypos;
};

struct E_GlfwCursorEnter {
  GLFWwindow* window;
  int entered;
};

struct E_GlfwMouseButton {
  GLFWwindow* window;
  int button;
  int action;
  int mods;
};

struct E_GlfwScroll {
  GLFWwindow* window;
  double xoffset;
  double yoffset;
};

struct E_GlfwJoystick {
  int jid;
  int event;
};

struct E_GlfwDrop {
  GLFWwindow* window;
  int count;
  const char** paths;
};

struct E_GlfwSetWindowSize {
  GLFWwindow* window;
  int width;
  int height;
};

struct E_GlfwSetWindowPos {
  GLFWwindow* window;
  int x;
  int y;
};

struct E_GlfwSetWindowTitle {
  GLFWwindow* window;
  std::string title;
};

struct E_GlfwSetWindowIcon {
  GLFWwindow* window;
  const std::vector<std::filesystem::path> paths;
};

/* MESSAGES */
} // namespace imp

#endif//IMPERATOR_CORE_HERMES_PAYLOADS_HPP
