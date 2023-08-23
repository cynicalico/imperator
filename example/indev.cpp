#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::CP437> font;

  std::shared_ptr<baphy::AbsoluteLayout> lay;
  std::shared_ptr<baphy::AbsoluteLayout> lay2;
  std::shared_ptr<baphy::AbsoluteLayout> lay3;
  std::shared_ptr<baphy::PrimitiveButton> b1;
  std::shared_ptr<baphy::PrimitiveButton> b2;
  std::shared_ptr<baphy::PrimitiveButton> b3;
  std::shared_ptr<baphy::PrimitiveButton> b4;
  std::shared_ptr<baphy::PrimitiveButton> b5;

  void initialize() override {
    cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();

    font_tex = textures->load(HERE / "res" / "img" / "chunky_8x9.png", true);
    font = fonts->load<baphy::CP437>("f", font_tex, 8, 9, 2);

    lay = gui->create<baphy::AbsoluteLayout>(50.0f, 100.0f, 400.0f, 200.0f);
    lay->set_show_border(true);
    lay->set_border_color(baphy::rgb("white"));

    b1 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 1", [&] { clicked_b1(); });
    lay->add(b1, 10, 10);

    lay2 = gui->create<baphy::AbsoluteLayout>(100.0f, 75.0f);
    lay2->set_show_border(true);
    lay2->set_border_color(baphy::rgb("lime"));

    b2 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 2", [&] { clicked_b2(); });
    lay2->add(b2, 10, 10);

    b3 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 3", [&] { clicked_b3(); });
    lay2->add(b3, b2->x(), b2->y() + b2->h() + 10);

    lay->add(lay2, b1->x(), b1->y() + b1->h() + 10);

    b4 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 4", [&] { clicked_b4(); });
    lay->add(b4, lay2->x(), lay2->y() + lay2->h() + 10);

    lay3 = gui->create<baphy::AbsoluteLayout>(lay->x(), lay->y() + lay->h() + 50.0f, 400.0f, 100.0f);
    lay3->set_show_border(true);
    lay3->set_border_color(baphy::rgb("white"));

    b5 = gui->create<baphy::PrimitiveButton>(*font, 1, "Test button 5", [&] { clicked_b5(); });
    lay3->add(b5, 10, 10);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x202020));

    lay->draw(0, 0);
    lay3->draw(0, 0);
  }

  void clicked_b1() {
    BAPHY_LOG_INFO("Clicked button 1!");
  }

  void clicked_b2() {
    BAPHY_LOG_INFO("Clicked button 2!");
  }

  void clicked_b3() {
    BAPHY_LOG_INFO("Clicked button 3!");
  }

  void clicked_b4() {
    BAPHY_LOG_INFO("Clicked button 4!");
  }

  void clicked_b5() {
    BAPHY_LOG_INFO("Clicked button 5!");
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
