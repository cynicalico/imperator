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
}

void Indev::draw() {
  ctx->gl.Viewport(0, 0, window->w(), window->h());
  gfx->clear(imp::rgb("black"));

  gfx->tri({300, 300}, {350, 350}, {300, 350}, imp::rgb("white"));

  gfx->rect({500, 500}, {100, 100}, imp::rgb("blue"));
  gfx->line({0, 0}, {inputs->mouse_x(), inputs->mouse_y()}, imp::rgb("lime"));
  gfx->rect({550, 550}, {100, 100}, imp::rgba("red", 128));

  gfx->point({100, 100}, imp::rgb("yellow"));

  gfx->batcher->draw(window->projection_matrix());
}

int main(int, char*[]) {
  imp::Engine().run_application<Indev>(imp::WindowOpenParams{
    .title = "Indev",
    .size = {1280, 720},
    .mode = imp::WindowMode::windowed,
    .flags = imp::WindowFlags::centered | imp::WindowFlags::vsync
  });
}
