#include "imperator/imperator.h"
#include "imperator/module/gfx/2d/g2d.h"

class Indev final : public imp::Application {
public:
  std::shared_ptr<imp::G2D> g2d;
  imp::HSV clear_color{imp::hsv(0.0f, 1.0f, 1.0f)};

  explicit Indev(imp::ModuleMgr &module_mgr) : Application(module_mgr) { g2d = module_mgr_.create<imp::G2D>(); }

  void update(double dt) override {
    if (dt == 0) return;

    clear_color.h = std::fmod(clear_color.h + (90.0f * dt), 360.0f);
  }

  void draw() override { g2d->clear(clear_color); }
};

int main(int, char *[]) {
  imp::mainloop<Indev>(
    imp::ApplicationParams{
      .window = {
        .title = "Indev",
        .size = {1280, 720},
        .mode = imp::WindowMode::windowed,
        .flags = imp::WindowFlags::centered | imp::WindowFlags::resizable
      },
      .gfx = {
        .backend_version = {3, 3},
        .vsync = false
      }
    }
  );
}
