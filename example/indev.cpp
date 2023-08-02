#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  float theta{0};
  float size_off{0};

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    theta += 10.0f * dt;
    size_off = 25 * std::sin(glm::radians(theta * 3));
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    primitives->fill_rect(100, 100, 50 + std::floor(size_off), 50 + std::floor(size_off), theta, baphy::rgb("blue"));
    primitives->draw_rect(100, 100, 50 + std::floor(size_off), 50 + std::floor(size_off), theta, baphy::rgb("white"));

    primitives->fill_tri(200, 100, 250 + std::floor(size_off), 100 + std::floor(size_off), 250 + std::floor(size_off), 150 + std::floor(size_off), theta, baphy::rgb("blue"));
    primitives->draw_tri(200, 100, 250 + std::floor(size_off), 100 + std::floor(size_off), 250 + std::floor(size_off), 150 + std::floor(size_off), theta, baphy::rgb("white"));

    primitives->fill_tri_equilateral(250, 350, 50 + std::floor(size_off), theta, baphy::rgb("blue"));
    primitives->draw_tri_equilateral(250, 350, 50 + std::floor(size_off), theta, baphy::rgb("white"));
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
