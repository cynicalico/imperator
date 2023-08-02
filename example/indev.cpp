#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> tex{nullptr};
  std::unique_ptr<baphy::Spritesheet> ssheet{nullptr};
  std::unique_ptr<baphy::ParticleSystem> ps{nullptr};
  std::vector<std::string> sprite_names = {"apple", "orange", "blueberry", "bananas", "cherry", "lemon", "lime"};

  std::shared_ptr<baphy::Texture> font_tex{nullptr};
  std::shared_ptr<baphy::Font> font{nullptr};

  float ps_x{0};

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();

    font_tex = textures->load(HERE / "res" / "img" / "1px_7x9.png", true);
    font = fonts->cp437("f", font_tex, 7, 9, 2);

    tex = textures->load(HERE / "res" / "img" / "fruits.png", true);
    ssheet = std::make_unique<baphy::Spritesheet>(tex, HERE / "res" / "img" / "fruits.json");
    ps = std::make_unique<baphy::ParticleSystem>(ssheet.get(), ssheet->sprite_names());
    ps->set_ttl(1, 5);
    ps->set_limit(200000);
    ps->set_pos(window->w() / 2, window->h() / 2);
    ps->set_rate(40000);
    ps->set_dir(90);
    ps->set_spread(360);
    ps->set_speed(50, 150);
    ps->set_linear_accel(0, 0, 200, 200);
    ps->set_radial_accel(0, 50);
    ps->set_tangent_accel(-100, 100);
    ps->set_linear_damping(0.5, 2);
    ps->set_spin(-720, 720);
    ps->set_colors({
      baphy::rgba(0xffffffff),
      baphy::rgba(0xffffffff),
      baphy::rgba(0x00000000)
    });
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    if (tween->finished("ps_move") || frameinfo.first)
      tween->begin("ps_move", ps_x, (ps_x == 0) ? window->w() : 0.0f, 1.0, baphy::Easing::linear, [&](const auto &v) { ps_x = v; });

    ps->move_to(ps_x, 0);
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    ps->draw();

    auto str = fmt::format("live particles: {}", ps->live_count());
    auto bb = font->bounds(1, str);

    primitives->rect(1, window->h() - bb.y - 3, bb.x + 2, bb.y + 2, baphy::rgba(0x00000080));
    font->draw(2, window->h() - bb.y - 2, 1, str);
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .flags = baphy::WindowFlags::borderless,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
