#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class IndevGui : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::CP437Font> font;

  std::shared_ptr<baphy::AbsoluteLayout> lay;
  std::shared_ptr<baphy::PrimitiveButton> b1;

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();

    font_tex = textures->load(HERE / "res" / "font" / "cp437" / "chunky_8x9.png", true);
    font = fonts->load<baphy::CP437Font>("f", font_tex, 8, 9, 2);

    lay = gui->create<baphy::AbsoluteLayout>(50.0f, 100.0f, 400.0f, 200.0f);
    lay->set_show_border(true);
    lay->set_border_color(baphy::rgb("white"));

    b1 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 1", [&] { clicked_b1(); });
    lay->add(b1, 10, 10);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x5698cc));

    lay->draw(0, 0);
  }

  void clicked_b1() {
    BAPHY_LOG_INFO("Clicked button 1!");
  }
};

BAPHY_RUN(IndevGui,
    .title = "IndevGui",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
