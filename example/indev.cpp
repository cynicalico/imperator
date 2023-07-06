#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  float x{0.0f};
  float y{0.0f};
  float w{50.0f};
  float h{50.0f};
  baphy::HSV color{baphy::hsv(0.0, 1.0, 1.0)};

  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    x = static_cast<float>(input->mouse_x());
    y = static_cast<float>(input->mouse_y());

    color.h = std::fmod(color.h + (90.0f * dt), 360.0f);
  }

  void draw() override {
    gfx->clear(baphy::rgba_f(0.06, 0.06, 0.06, 0.0));

    auto c = color.to_rgb().vec4();
    baphy::EventBus::send_nowait<baphy::EOPrimitiveVertexData>(std::vector{
        x - (w / 2.0f), y - (h / 2.0f), 2.0f,  c.r, c.g, c.b, c.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), 2.0f,  c.r, c.g, c.b, c.a,  0.0f, 0.0f, 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), 2.0f,  c.r, c.g, c.b, c.a,  0.0f, 0.0f, 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), 2.0f,  c.r, c.g, c.b, c.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y + (h / 2.0f), 2.0f,  c.r, c.g, c.b, c.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), 2.0f,  c.r, c.g, c.b, c.a,  0.0f, 0.0f, 0.0f,
    }, 2.0f);
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 800},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
