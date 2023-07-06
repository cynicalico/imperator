#ifndef BAPHY_CORE_MSGS_HPP
#define BAPHY_CORE_MSGS_HPP

#define GLFW_INCLUDE_NONE
#include "baphy/core/msgs_types.hpp"
#include "GLFW/glfw3.h"
#include "spdlog/common.h"
#include <cstdint>
#include <memory>
#include <string>

// This gets defined when we set the spdlog level, but the intent is for
// it to get defined in the log.hpp header
#undef SPDLOG_ACTIVE_LEVEL

namespace baphy {

struct EBase {
  virtual ~EBase() = default;
};

struct EInitialize : public EBase {
  const WindowOpenParams window_open_params;

  EInitialize(const WindowOpenParams &window_open_params) : window_open_params(window_open_params) {}
};

struct EShutdown : public EBase {};

struct EStartApplication : public EBase {};

struct EUpdate : public EBase  {
  double dt;
  double fps;
  std::uint64_t timestamp;

  EUpdate(double dt, double fps, std::uint64_t timestamp) : dt(dt), fps(fps), timestamp(timestamp) {}
};

struct EStartFrame : public EBase  {};

struct EDraw : public EBase  {};

struct EEndFrame : public EBase  {};

struct ELogMsg : public EBase  {
  std::string text;
  spdlog::level::level_enum level;

  ELogMsg(const std::string &text, spdlog::level::level_enum level) : text(text), level(level) {}
};

/* GLFW CALLBACKS */

struct EGlfwWindowClose : public EBase {
  GLFWwindow *window;

  EGlfwWindowClose(GLFWwindow *window) : window(window) {}
};

struct EGlfwWindowSize : public EBase {
  GLFWwindow *window;
  int width;
  int height;

  EGlfwWindowSize(GLFWwindow *window, int width, int height) : window(window), width(width), height(height) {}
};

struct EGlfwFramebufferSize : public EBase {
  GLFWwindow *window;
  int width;
  int height;

  EGlfwFramebufferSize(GLFWwindow *window, int width, int height) : window(window), width(width), height(height) {}
};

struct EGlfwWindowContentScale : public EBase {
  GLFWwindow *window;
  float xscale;
  float yscale;

  EGlfwWindowContentScale(GLFWwindow *window, float xscale, float yscale) : window(window), xscale(xscale), yscale(yscale) {}
};

struct EGlfwWindowPos : public EBase {
  GLFWwindow *window;
  int xpos;
  int ypos;

  EGlfwWindowPos(GLFWwindow *window, int xpos, int ypos) : window(window), xpos(xpos), ypos(ypos) {}
};

struct EGlfwWindowIconify : public EBase {
  GLFWwindow *window;
  int iconified;

  EGlfwWindowIconify(GLFWwindow *window, int iconified) : window(window), iconified(iconified) {}
};

struct EGlfwWindowMaximize : public EBase {
  GLFWwindow *window;
  int maximized;

  EGlfwWindowMaximize(GLFWwindow *window, int maximized) : window(window), maximized(maximized) {}
};

struct EGlfwWindowFocus : public EBase {
  GLFWwindow *window;
  int focused;

  EGlfwWindowFocus(GLFWwindow *window, int focused) : window(window), focused(focused) {}
};

struct EGlfwWindowRefresh : public EBase {
  GLFWwindow *window;

  EGlfwWindowRefresh(GLFWwindow *window) : window(window) {}
};

struct EGlfwMonitor : public EBase {
  GLFWmonitor *monitor;
  int event;

  EGlfwMonitor(GLFWmonitor *monitor, int event) : monitor(monitor), event(event) {}
};

struct EGlfwKey : public EBase {
  GLFWwindow *window;
  int key;
  int scancode;
  int action;
  int mods;

  EGlfwKey(GLFWwindow *window, int key, int scancode, int action, int mods) : window(window), key(key), scancode(scancode), action(action), mods(mods) {}
};

struct EGlfwCharacter : public EBase {
  GLFWwindow *window;
  unsigned int codepoint;

  EGlfwCharacter(GLFWwindow *window, unsigned int codepoint) : window(window), codepoint(codepoint) {}
};

struct EGlfwCursorPos : public EBase {
  GLFWwindow *window;
  double xpos;
  double ypos;

  EGlfwCursorPos(GLFWwindow *window, double xpos, double ypos) : window(window), xpos(xpos), ypos(ypos) {}
};

struct EGlfwCursorEnter : public EBase {
  GLFWwindow *window;
  int entered;

  EGlfwCursorEnter(GLFWwindow *window, int entered) : window(window), entered(entered) {}
};

struct EGlfwMouseButton : public EBase {
  GLFWwindow *window;
  int button;
  int action;
  int mods;

  EGlfwMouseButton(GLFWwindow *window, int button, int action, int mods) : window(window), button(button), action(action), mods(mods) {}
};

struct EGlfwScroll : public EBase {
  GLFWwindow *window;
  double xoffset;
  double yoffset;

  EGlfwScroll(GLFWwindow *window, double xoffset, double yoffset) : window(window), xoffset(xoffset), yoffset(yoffset) {}
};

struct EGlfwJoystick : public EBase {
  int jid;
  int event;

  EGlfwJoystick(int jid, int event) : jid(jid), event(event) {}
};

struct EGlfwDrop : public EBase {
  GLFWwindow *window;
  int count;
  const char **paths;

  EGlfwDrop(GLFWwindow *window, int count, const char **paths) : window(window), count(count), paths(paths) {}
};

} // namespace baphy

#endif//BAPHY_CORE_MSGS_HPP
