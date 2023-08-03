#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::Font> font;

  std::shared_ptr<baphy::AbsoluteLayout> lay;
  std::shared_ptr<baphy::PrimitiveButton> b1;
  std::shared_ptr<baphy::PrimitiveButton> b2;

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();

    font_tex = textures->load(HERE / "res" / "img" / "1px_7x9.png", true);
    font = fonts->cp437("f", font_tex, 7, 9, 2);

    lay = gui->create_layout<baphy::AbsoluteLayout>(300, 300);
    lay->set_show_border(true);
    lay->set_border_color(baphy::rgb("white"));

    b1 = lay->create_node<baphy::PrimitiveButton>(0, 0, *font, 2, "Test button 1", [&] {
      BAPHY_LOG_INFO("Clicked button 1!");
    });
    b1->set_border_color(baphy::rgb("blue"));
    b1->set_bg_color(baphy::rgb("red"));
    b1->set_fg_color(baphy::rgb("yellow"));

    b2 = lay->create_node<baphy::PrimitiveButton>(0, b1->h() + 2, *font, 2, "Test button 2", [&] {
      BAPHY_LOG_INFO("Clicked button 2!");
    });
    b2->set_border_color(baphy::rgb("green"));
    b2->set_bg_color(baphy::rgb("red"));
    b2->set_fg_color(baphy::rgb("yellow"));
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    lay->draw(100, 100);
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
