#include "baphy/baphy.hpp"

struct Rect {
  float x, y, w, h, z;
  baphy::RGB color;
};

class Indev : public baphy::Application {
public:
  float z = 1.0f;
  std::vector<Rect> rects{};
  std::shared_ptr<baphy::Batcher> batcher{nullptr};

  double theta = 0.0;

  void initialize() override {
    rects.reserve(100000);
    for (std::size_t i = 0; i < 100000; i++) {
      rects.emplace_back(Rect {
          baphy::get<float>(window->w() / 4.0f, 3.0f * window->w() / 4.0f),
          baphy::get<float>(window->h() / 4.0f, 3.0f * window->h() / 4.0f),
          baphy::get<float>(10.0f, 50.0f),
          baphy::get<float>(10.0f, 50.0f),
          z,
          baphy::rgb_f(baphy::get<float>(0.5f), baphy::get<float>(0.5f), 0.5f)
      });
      z += 1.0f;
    }

    batcher = module_mgr->get<baphy::Batcher>();
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    theta = std::fmod(theta + (180.0 * dt), 360.0);
  }

  inline void draw_rect(float x, float y, float w, float h, float z, const baphy::RGB &c) {
    auto cv = c.vec4();
    batcher->add_o_primitive({
        x - (w / 2.0f), y - (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y + (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
    }, z);
  }

  void draw() override {
    gfx->clear(baphy::rgba_f(0.06, 0.06, 0.06, 0.0));

    for (const auto &r: rects) {
      auto &[x, y, w, h, z, c] = r;
      draw_rect(x, y, w, h, z, c);
    }

    auto x = (window->w() / 2.0f) + ((window->w() / 3.5f) * std::sin(glm::radians(theta)));
    auto y = (window->h() / 2.0f) + ((window->h() / 3.5f) * std::cos(glm::radians(theta)));
    draw_rect(x, y, 50.0f, 50.0f, z, baphy::rgb("lightgray"));
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 600},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered
)
