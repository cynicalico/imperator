#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> tex{nullptr};
  int tx{0};
  int ty{0};
  float theta{0.0f};
  float n{2.0f};

  void initialize() override {
    input->hide_cursor();

    tex = textures->load(HERE / "res" / "img" / "fruits.png", true);

    timer->every(1.0, [&]{
      tx = baphy::rnd::get<int>(5);
      ty = baphy::rnd::get<int>(5);
    });

    debug->set_cmd_key("1");
    debug->set_cmd_callback("n", [&](const auto &cmd) {
      n = baphy::stof(cmd).value_or(n);
    });
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    theta += dt * 90.0f;
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    float x = (window->w() / 4) * std::sin(glm::radians(theta * n));
    float y = (window->h() / 4) * std::cos(glm::radians(theta));
    float w = 16 * 8;
    float h = 16 * 8;
    tex->draw((window->w() / 2) + x - (w / 2), (window->h() / 2) + y - (h / 2), w, h, tx * 16, ty * 16, 16, 16, theta / 4);
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
