#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgba(0x00000000));

    primitives->tri_equilateral(100, 100, 100, baphy::rgba("red", 128));
    primitives->tri_equilateral(input->mouse_x(), input->mouse_y(), 100, baphy::rgb("yellow"));
    primitives->tri_equilateral(400, 400, 100, baphy::rgba("green", 128));
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
