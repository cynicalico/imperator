#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::CP437> font;

  std::shared_ptr<baphy::AbsoluteLayout> lay;
  std::shared_ptr<baphy::AbsoluteLayout> lay2;
  std::shared_ptr<baphy::PrimitiveButton> b1;
  std::shared_ptr<baphy::PrimitiveButton> b2;

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();

    font_tex = textures->load(HERE / "res" / "img" / "chunky_8x9.png", true);
    font = fonts->load<baphy::CP437>("f", font_tex, 8, 9, 2);

    lay = gui->create<baphy::AbsoluteLayout>(1.0f, 1.0f, window->w() - 2.0f, window->h() - 2.0f);
    lay->set_show_border(true);
    lay->set_border_color(baphy::rgb("white"));

    b1 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 1", [&] { clicked_b1(); });
    lay->add(b1, 100, 100);

    lay2 = gui->create<baphy::AbsoluteLayout>(100, 100);
    lay2->set_show_border(true);
    lay2->set_border_color(baphy::rgb("lime"));

    b2 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 2", [&] { clicked_b2(); });
    lay2->add(b2, 5, 5);

    lay->add(lay2, b1->x(), b1->y() + b1->h() + 10);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x202020));

    lay->draw();
  }

  void clicked_b1() {
    BAPHY_LOG_INFO("Clicked button 1!");
  }

  void clicked_b2() {
    BAPHY_LOG_INFO("Clicked button 2!");
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
