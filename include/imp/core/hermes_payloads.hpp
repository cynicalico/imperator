#ifndef IMP_CORE_HERMES_PAYLOADS_HPP
#define IMP_CORE_HERMES_PAYLOADS_HPP

#define GLFW_INCLUDE_NONE
#include "imp/core/hermes_payloads_types.hpp"
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

#define IMP_DECLARE_PAYLOAD(struct_name, ...)       \
  struct struct_name {                              \
    __VA_ARGS__                                     \
  };                                                \
  template<> struct imp::PayloadInfo<struct_name> { \
    static constexpr auto name = #struct_name;      \
  };

namespace imp {
/* EVENTS */

IMP_DECLARE_PAYLOAD(E_Initialize,
  const InitializeParams params;
)

IMP_DECLARE_PAYLOAD(E_ShutdownEngine)

IMP_DECLARE_PAYLOAD(E_Shutdown)

IMP_DECLARE_PAYLOAD(E_LogMsg,
  std::string text;
  spdlog::level::level_enum level;
)

IMP_DECLARE_PAYLOAD(E_StartFrame)

IMP_DECLARE_PAYLOAD(E_Draw)

IMP_DECLARE_PAYLOAD(E_EndFrame)

IMP_DECLARE_PAYLOAD(E_Update,
  double dt;
  double fps;
);

IMP_DECLARE_PAYLOAD(E_GlfwWindowClose,
  GLFWwindow* window;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowSize,
  GLFWwindow* window;
  int width;
  int height;
)

IMP_DECLARE_PAYLOAD(E_GlfwFramebufferSize,
  GLFWwindow* window;
  int width;
  int height;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowContentScale,
  GLFWwindow* window;
  float xscale;
  float yscale;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowPos,
  GLFWwindow* window;
  int xpos;
  int ypos;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowIconify,
  GLFWwindow* window;
  int iconified;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowMaximize,
  GLFWwindow* window;
  int maximized;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowFocus,
  GLFWwindow* window;
  int focused;
)

IMP_DECLARE_PAYLOAD(E_GlfwWindowRefresh,
  GLFWwindow* window;
)

IMP_DECLARE_PAYLOAD(E_GlfwMonitor,
  GLFWmonitor* monitor;
  int event;
)

IMP_DECLARE_PAYLOAD(E_GlfwKey,
  GLFWwindow* window;
  int key;
  int scancode;
  int action;
  int mods;
)

IMP_DECLARE_PAYLOAD(E_GlfwCharacter,
  GLFWwindow* window;
  unsigned int codepoint;
)

IMP_DECLARE_PAYLOAD(E_GlfwCursorPos,
  GLFWwindow* window;
  double xpos;
  double ypos;
)

IMP_DECLARE_PAYLOAD(E_GlfwCursorEnter,
  GLFWwindow* window;
  int entered;
)

IMP_DECLARE_PAYLOAD(E_GlfwMouseButton,
  GLFWwindow* window;
  int button;
  int action;
  int mods;
)

IMP_DECLARE_PAYLOAD(E_GlfwScroll,
  GLFWwindow* window;
  double xoffset;
  double yoffset;
)

IMP_DECLARE_PAYLOAD(E_GlfwJoystick,
  int jid;
  int event;
)

IMP_DECLARE_PAYLOAD(E_GlfwDrop,
  GLFWwindow* window;
  int count;
  const char** paths;
)

IMP_DECLARE_PAYLOAD(E_GlfwSetWindowSize,
  GLFWwindow* window;
  int width;
  int height;
)

IMP_DECLARE_PAYLOAD(E_GlfwSetWindowPos,
  GLFWwindow* window;
  int x;
  int y;
)

IMP_DECLARE_PAYLOAD(E_GlfwSetWindowTitle,
  GLFWwindow* window;
  std::string title;
)

IMP_DECLARE_PAYLOAD(E_GlfwSetWindowIcon,
  GLFWwindow* window;
  const std::vector<std::filesystem::path> paths;
)

/* MESSAGES */
} // namespace imp

#endif//IMP_CORE_HERMES_PAYLOADS_HPP
