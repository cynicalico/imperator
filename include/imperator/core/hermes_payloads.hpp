#ifndef IMPERATOR_CORE_HERMES_PAYLOADS_HPP
#define IMPERATOR_CORE_HERMES_PAYLOADS_HPP

#define GLFW_INCLUDE_NONE
#include "imperator/core/hermes_payloads_types.hpp"
#include "GLFW/glfw3.h"
#include "spdlog/common.h"
#include <filesystem>
#include <string>

// This gets defined when we include spdlog/common.hpp, but the
// intent is for it to get defined in the log.hpp header (or by
// the user)
#undef SPDLOG_ACTIVE_LEVEL

namespace imp {
template<typename>
struct PayloadInfo {
  static constexpr auto name = "???";
};
} // namespace imp

#define IMP_DECLARE_PAYLOAD(payload)      \
  template<> struct imp::PayloadInfo<payload> { \
    static constexpr auto name = #payload;      \
  }

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
  double fps;
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

IMP_DECLARE_PAYLOAD(imp::E_Initialize);
IMP_DECLARE_PAYLOAD(imp::E_ShutdownEngine);
IMP_DECLARE_PAYLOAD(imp::E_Shutdown);
IMP_DECLARE_PAYLOAD(imp::E_LogMsg);
IMP_DECLARE_PAYLOAD(imp::E_StartFrame);
IMP_DECLARE_PAYLOAD(imp::E_Draw);
IMP_DECLARE_PAYLOAD(imp::E_EndFrame);
IMP_DECLARE_PAYLOAD(imp::E_Update);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowClose);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowSize);
IMP_DECLARE_PAYLOAD(imp::E_GlfwFramebufferSize);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowContentScale);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowPos);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowIconify);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowMaximize);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowFocus);
IMP_DECLARE_PAYLOAD(imp::E_GlfwWindowRefresh);
IMP_DECLARE_PAYLOAD(imp::E_GlfwMonitor);
IMP_DECLARE_PAYLOAD(imp::E_GlfwKey);
IMP_DECLARE_PAYLOAD(imp::E_GlfwCharacter);
IMP_DECLARE_PAYLOAD(imp::E_GlfwCursorPos);
IMP_DECLARE_PAYLOAD(imp::E_GlfwCursorEnter);
IMP_DECLARE_PAYLOAD(imp::E_GlfwMouseButton);
IMP_DECLARE_PAYLOAD(imp::E_GlfwScroll);
IMP_DECLARE_PAYLOAD(imp::E_GlfwJoystick);
IMP_DECLARE_PAYLOAD(imp::E_GlfwDrop);
IMP_DECLARE_PAYLOAD(imp::E_GlfwSetWindowSize);
IMP_DECLARE_PAYLOAD(imp::E_GlfwSetWindowPos);
IMP_DECLARE_PAYLOAD(imp::E_GlfwSetWindowTitle);
IMP_DECLARE_PAYLOAD(imp::E_GlfwSetWindowIcon);

#endif//IMPERATOR_CORE_HERMES_PAYLOADS_HPP
