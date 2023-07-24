#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> tex{nullptr};
  int tx{0};
  int ty{0};
  float theta{0.0f};

  void initialize() override {
    input->hide_cursor();

    tex = textures->load(HERE / "res" / "img" / "fruits.png", true);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    if (input->pressed("mb_left")) {
      tx = baphy::get<int>(5);
      ty = baphy::get<int>(5);
    }

    theta += dt * 90.0f;
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    float w = 16 * 8;
    float h = 16 * 8;
    tex->draw(input->mouse_x() - w, input->mouse_y() - h, w, h, tx * 16, ty * 16, 16, 16, theta);
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 720},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
