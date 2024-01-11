#include "imperator/core/engine.hpp"

#include "imperator/util/io.hpp"

namespace imp {
Engine::Engine() {
  module_mgr_ = std::make_shared<ModuleMgr>();

  Hermes::sub<E_ShutdownEngine>(EPI<Engine>::name, [&](const auto&) {
    received_shutdown_ = true;
  });
}

bool Engine::check_pending_() {
  bool no_pending = true;

#define CHECK(payload)                                                                       \
  if (Hermes::has_pending<payload>()) {                                                      \
    auto v = Hermes::get_pending<payload>();                                                 \
    for (const auto& [name, pending]: v) {                                                   \
      IMP_LOG_ERROR("{}: {} relies on {}", name, PayloadInfo<payload>::name, pending); \
    }                                                                                        \
    no_pending = false;                                                                      \
  }

  CHECK(E_Initialize)
  CHECK(E_ShutdownEngine)
  CHECK(E_Shutdown)
  CHECK(E_LogMsg)
  CHECK(E_StartFrame)
  CHECK(E_Draw)
  CHECK(E_EndFrame)
  CHECK(E_Update)
  CHECK(E_GlfwWindowClose)
  CHECK(E_GlfwWindowSize)
  CHECK(E_GlfwFramebufferSize)
  CHECK(E_GlfwWindowContentScale)
  CHECK(E_GlfwWindowPos)
  CHECK(E_GlfwWindowIconify)
  CHECK(E_GlfwWindowMaximize)
  CHECK(E_GlfwWindowFocus)
  CHECK(E_GlfwWindowRefresh)
  CHECK(E_GlfwMonitor)
  CHECK(E_GlfwKey)
  CHECK(E_GlfwCharacter)
  CHECK(E_GlfwCursorPos)
  CHECK(E_GlfwCursorEnter)
  CHECK(E_GlfwMouseButton)
  CHECK(E_GlfwScroll)
  CHECK(E_GlfwJoystick)
  CHECK(E_GlfwDrop)
  CHECK(E_GlfwSetWindowSize)
  CHECK(E_GlfwSetWindowPos)
  CHECK(E_GlfwSetWindowTitle)
  CHECK(E_GlfwSetWindowIcon)

#undef CHECK

  return no_pending;
}
} // namespace imp
