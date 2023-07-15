#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  float theta{0.0f};

  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    theta += 90.0 * dt;
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    primitives->tri_equilateral(window->w() / 2, window->h() / 4, window->h() / 2, 90, baphy::rgb("red"));
    primitives->tri_equilateral(input->mouse_x(), input->mouse_y(), window->h() / 4, theta, baphy::rgba("blue", 128));
    primitives->tri_equilateral(window->w() / 2, 3 * window->h() / 4, window->h() / 2, 270, baphy::rgb("lime"));

    primitives->line(0, 0, input->mouse_x(), input->mouse_y(), baphy::rgba("yellow", 128));
    primitives->line(window->w(), 0, input->mouse_x(), input->mouse_y(), baphy::rgba("yellow", 128));
    primitives->line(window->w(), window->h(), input->mouse_x(), input->mouse_y(), baphy::rgba("yellow", 128));
    primitives->line(0, window->h(), input->mouse_x(), input->mouse_y(), baphy::rgba("yellow", 128));
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 960},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_options.json"
)
