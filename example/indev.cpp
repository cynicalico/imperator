#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::CP437Font> font;

  std::shared_ptr<baphy::Texture> font_tex2;
  std::shared_ptr<baphy::CP437Font> font2;

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();

    font_tex = textures->load(HERE / "res" / "font" / "cp437_prop" / "dmf_calligraphy_11.png", true);
    font = fonts->load<baphy::CP437Font>("fantasy", font_tex, HERE / "res" / "font" / "cp437_prop" / "dmf_calligraphy_11.txt", 11, 2);

    font_tex2 = textures->load(HERE / "res" / "font" / "cp437" / "dmf_calligraphy_8x8.png", true);
    font2 = fonts->load<baphy::CP437Font>("fantasy2", font_tex2, 8, 8, 2);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x000000));

    auto s1 = std::string(R"(
This text is written in a proportional font!
The characters are on a font atlas that is *nearly* Code Page 437,
but without a fixed width.
)");
    s1 = s1.substr(1, s1.size() - 2);

    auto s2 = std::string(R"(
This text is written in a proportional font!
The characters are on a font atlas that is *nearly* Code Page 437,
but without a fixed width.
)");
    s2 = s2.substr(1, s2.size() - 2);

    auto s1_bounds = font->bounds(2, s1);
    primitives->draw_rect(100, 100, s1_bounds.x, s1_bounds.y, baphy::rgb("green"));
    font->draw(100, 100, 2, s1, baphy::rgb(0xffffff));

    auto s2_bounds = font2->bounds(2, s2);
    primitives->draw_rect(100, 200, s2_bounds.x, s2_bounds.y, baphy::rgb("green"));
    font2->draw(100, 200, 2, s2, baphy::rgb(0xffffff));
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 720},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
