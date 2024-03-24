#include "imperator/imperator.h"

class Indev final : public imp::Application {
public:
  explicit Indev(std::weak_ptr<imp::ModuleMgr> module_mgr)
    : Application(std::move(module_mgr)) {
  }
};

int main(int, char*[]) {
  imp::mainloop<Indev>(imp::WindowOpenParams{
    .title = "Indev",
    .size = {1280, 720},
    .mode = imp::WindowMode::windowed,
    .flags = imp::WindowFlags::centered
  });
}
