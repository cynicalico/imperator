#include "indev.hpp"

const auto CWD = std::filesystem::current_path();

class Indev : public imp::Application {
public:
  std::shared_ptr<imp::Gfx2D> gfx{nullptr};

  std::shared_ptr<imp::Texture> bulbasaur{nullptr};
  std::shared_ptr<imp::Texture> charmander{nullptr};
  std::shared_ptr<imp::Texture> squirtle{nullptr};

  explicit Indev(const std::weak_ptr<imp::ModuleMgr>& module_mgr);

  void update(double dt) override;
  void draw() override;
};

Indev::Indev(const std::weak_ptr<imp::ModuleMgr>& module_mgr) : Application(module_mgr) {
  debug_overlay->set_flying_log_enabled(true);
  debug_overlay->set_console_binding("grave_accent");

  gfx = module_mgr.lock()->create<imp::Gfx2D>();

  bulbasaur = gfx->textures->load(CWD / "example" / "res" / "img" / "bulbasaur.png", true);
  charmander = gfx->textures->load(CWD / "example" / "res" / "img" / "charmander.png", true);
  squirtle = gfx->textures->load(CWD / "example" / "res" / "img" / "squirtle.png", true);
}

void Indev::update(double dt) {
  if (inputs->pressed("escape")) {
    window->set_should_close(true);
  }
}

void Indev::draw() {
  ctx->gl.Viewport(0, 0, window->w(), window->h());
  gfx->clear(imp::rgb("black"));

  gfx->draw_tex(*bulbasaur, {100, 100});
  gfx->draw_tex(*charmander, {110 + bulbasaur->w(), 100});
  gfx->draw_tex(*squirtle, {120 + bulbasaur->w() + charmander->w(), 100});

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
