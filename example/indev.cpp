#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> tex{nullptr};
  std::shared_ptr<baphy::Surface> surf{nullptr};

  void initialize() override {
    tex = textures->load(HERE / "res" / "img" / "tile005.png", true);
    surf = surfaces->create(48 * 2, 48 * 2);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    surf->draw_on([&]{
      gfx->clear(baphy::rgb("green"));
      primitives->tri_equilateral(50, 50, 20, 90, baphy::rgb("blue"));
    });

    gfx->clear(baphy::rgba(0xff0000ff));

    tex->draw((window->w() / 2) - (tex->width / 2), (window->h() / 2) - (tex->height / 2));
    surf->draw(input->mouse_x(), input->mouse_y());
    primitives->tri_equilateral(3 * window->w() / 4, 3 * window->h() / 4, 100, 90, baphy::rgba("yellow", 128));
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
