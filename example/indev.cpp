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

    std::size_t sx = window->w() / 4;
    std::size_t ex = 3 * window->w() / 4;
    std::size_t sy = window->h() / 4;
    std::size_t ey = 3 * window->h() / 4;
    for (std::size_t x = sx; x < ex; x++)
      for (std::size_t y = sy; y < ey; y++) {
        double v = baphy::opensimplex::octave3d_improve_xy(x / (double)(ex - sx), y / (double)(ey - sy), z, 3, 0.2);
        primitives->point(x, y, baphy::rgb(v * 255, v * 255, v * 255));
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
