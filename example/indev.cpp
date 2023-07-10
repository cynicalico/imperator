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
    batcher = module_mgr->get<baphy::Batcher>();

    timer->every(0.05, [&]{
      rects.emplace_back(rand_rect());
      z += 1.0f;
    });
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    theta = std::fmod(theta + (180.0 * dt), 360.0);
  }

  inline Rect rand_rect() {
    return {
        baphy::get<float>(0.0f, window->w()),
        baphy::get<float>(0.0f, window->h()),
        baphy::get<float>(10.0f, 50.0f),
        baphy::get<float>(10.0f, 50.0f),
        z,
        baphy::rgb_f(baphy::get<float>(0.5f), baphy::get<float>(0.5f), 0.5f)
    };
  }

  inline void draw_rect(float x, float y, float w, float h, float z, const baphy::RGB &c) {
    auto cv = c.vec4();
    batcher->add_o_tri(z, {
        x - (w / 2.0f), y - (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y + (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
    });
  }

  inline void draw_line(float x0, float y0, float x1, float y1, float z, const baphy::RGB &c) {
    auto cv = c.vec4();
    batcher->add_o_line(z, {
        x0, y0, z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
        x1, y1, z,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
    });
  }

  inline void draw_point(float x, float y, float z, const baphy::RGB &c) {
    auto cv = c.vec4();
    batcher->add_o_point(z, {x, y, z,  cv.r, cv.g, cv.b, cv.a});
  }

  void draw() override {
    gfx->clear(baphy::rgba_f(0.06, 0.06, 0.06, 0.0));

    for (const auto &r: rects) {
      auto &[x, y, w, h, z, c] = r;
      draw_rect(x, y, w, h, z, c);
    }

    draw_line(0, 0, window->w() - 1, window->h() - 1, z + 1, baphy::rgb("red"));
    draw_line(0, window->h() - 1, window->w() - 1, 0, z + 2, baphy::rgb("red"));
    draw_line(0, 0, window->w() - 1, 0, z + 3, baphy::rgb("red"));
    draw_line(window->w() - 1, 0, window->w() - 1, window->h() - 1, z + 4, baphy::rgb("red"));
    draw_line(window->w() - 1, window->h() - 1, 0, window->h() - 1, z + 5, baphy::rgb("red"));
    draw_line(0, window->h() - 1, 0, 0, z + 6, baphy::rgb("red"));
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 600},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered
)
