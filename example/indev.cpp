#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Surface> surf{nullptr};

  void initialize() override {
    surf = surfaces->create(150, 150);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgba(0x00000000));

    primitives->tri_equilateral(window->w() / 2, window->h() / 2, 100, baphy::rgb("yellow"));

    surf->draw_on([&] {
      gfx->clear(baphy::rgba("blue", 128));
      primitives->tri_equilateral(75, 75, 50, baphy::rgb("lime"));
      primitives->line(0, 0, 150, 150, baphy::rgb("lime"));
    });
    surf->draw(input->mouse_x() - 75, input->mouse_y() - 75);

    primitives->tri_equilateral(window->w() / 4, window->h() / 4, 100, baphy::rgb("magenta"));
    primitives->tri_equilateral(3 * window->w() / 4, 3 * window->h() / 4, 100, baphy::rgba("magenta", 128));
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 960},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
