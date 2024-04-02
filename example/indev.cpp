#include "imperator/imperator.h"

class Indev final : public imp::Application {
public:
  explicit Indev(imp::ModuleMgr& module_mgr)
    : Application(module_mgr) {}

  void update(double dt) override {}

  void draw() override {
    ctx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ctx->gl.Clear(GL_COLOR_BUFFER_BIT);
  }
};

int main(int, char*[]) {
  imp::mainloop<Indev>(
    imp::ApplicationParams{
      .window = {
        .title = "Indev",
        .size = {1280, 720},
        .mode = imp::WindowMode::windowed,
        .flags = imp::WindowFlags::centered
      },
      .gfx = {
        .backend_version = {3, 3},
        .vsync = false
      }
    }
  );
}
