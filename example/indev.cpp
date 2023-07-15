#include "baphy/baphy.hpp"

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
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 600},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
