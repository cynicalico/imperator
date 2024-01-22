#include "indev.hpp"

const auto CWD = std::filesystem::current_path();

class Indev : public imp::Application {
public:
  std::shared_ptr<imp::Gfx2D> gfx{nullptr};

  explicit Indev(const std::weak_ptr<imp::ModuleMgr>& module_mgr);

  void update(double dt) override;
  void draw() override;
};

Indev::Indev(const std::weak_ptr<imp::ModuleMgr>& module_mgr) : Application(module_mgr) {
  debug_overlay->set_flying_log_enabled(true);
  debug_overlay->set_console_binding("grave_accent");

  gfx = module_mgr.lock()->create<imp::Gfx2D>();
}

void Indev::update(double dt) {
  if (inputs->pressed("escape")) {
    window->set_should_close(true);
  }

  if (inputs->pressed("1")) {
    gfx->say_hello();
  }
}

void Indev::draw() {
  ctx->clear(imp::rgb("black"));
}

int main(int, char*[]) {
  imp::Engine().run_application<Indev>(imp::WindowOpenParams{
    .title = "Indev",
    .size = {1280, 720},
    .mode = imp::WindowMode::windowed,
    .flags = imp::WindowFlags::centered | imp::WindowFlags::vsync
  });
}
