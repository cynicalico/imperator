#include "imperator/imperator.h"

class Indev : public imp::Application {
public:
  explicit Indev(std::weak_ptr<imp::ModuleMgr> module_mgr)
    : Application(std::move(module_mgr)) {}
};

namespace imp {
template<typename T>
  requires std::derived_from<T, Application>
void mainloop(WindowOpenParams params) {
  register_glfw_error_callback();
  if (!glfwInit())
    std::exit(EXIT_FAILURE);

  int major, minor, rev;
  glfwGetVersion(&major, &minor, &rev);
  IMPERATOR_LOG_DEBUG("Initialized GLFW v{}.{}.{}", major, minor, rev);

  const auto module_mgr = std::make_shared<ModuleMgr>();
  /* scope to destruct module refs */ {
    const auto event_bus = module_mgr->create<EventBus>();
    set_global_user_pointer(event_bus.get());

    const auto window = module_mgr->create<Window>(params);
    module_mgr->create<Application, T>();

    while (!window->should_close()) {
      event_bus->send_nowait<E_Update>();

      glfwPollEvents();
    }

    clear_global_user_pointer();
  }
  module_mgr->clear();

  glfwTerminate();
  IMPERATOR_LOG_DEBUG("Terminated GLFW");
}
} // namespace imp

int main(int, char*[]) {
  imp::mainloop<Indev>(imp::WindowOpenParams{
    .title = "Indev",
    .size = {1280, 720},
    .mode = imp::WindowMode::windowed,
    .flags = imp::WindowFlags::centered
  });
}
