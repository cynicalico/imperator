#include "baphy/baphy.hpp"
#include "baphy/util/noise/opensimplex.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  double z{0};

  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    z += dt / 2;
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    double s = 4;
    for (std::size_t x = 0; x < window->w(); x += s)
      for (std::size_t y = 0; y < window->h(); y += s) {
        double v = baphy::opensimplex::octave3d_improve_xy((x + (s / 2)) / (double)window->w(), (y + (s / 2)) / (double)window->h(), z, 3, 0.2);
        primitives->square(x, y, s, baphy::rgb(v * 255, v * 255, v * 255));
      }
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 960},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
