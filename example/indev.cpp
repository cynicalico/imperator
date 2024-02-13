#include "indev.hpp"

const auto CWD = std::filesystem::current_path();
const auto IMG_PATH = CWD / "example" / "res" / "img";

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

  bulbasaur = gfx->textures->load(IMG_PATH / "bulbasaur.png", true);
  charmander = gfx->textures->load(IMG_PATH / "charmander.png", true);
  squirtle = gfx->textures->load(IMG_PATH / "squirtle.png", true);
}

void Indev::update(double dt) {
  if (inputs->pressed("escape")) {
    window->set_should_close(true);
  }
}

void Indev::draw() {
  ctx->gl.Viewport(0, 0, window->w(), window->h());
  gfx->clear(imp::rgb("black"));

  gfx->draw_rect({100, 100}, {bulbasaur->w() - 1, bulbasaur->h() - 1}, imp::rgb("white"));
  gfx->draw_tex(*bulbasaur, {100, 100});

  gfx->draw_tex(*charmander, {110 + bulbasaur->w(), 100});

  gfx->line({0, 0}, {inputs->mouse_x(), inputs->mouse_y()}, imp::rgb("yellow"));

  gfx->draw_tex(*squirtle, {120 + bulbasaur->w() + charmander->w(), 100});

  gfx->draw_rect({0, 0}, {window->w() - 1, window->h() - 1}, imp::rgb("yellow"));

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
