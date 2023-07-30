#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> tex{nullptr};
  std::shared_ptr<baphy::Font> font{nullptr};

  void initialize() override {
    tex = textures->load(HERE / "res" / "img" / "phantasm_10x10.png", true);
    font = fonts->cp437(baphy::rnd::base58(11), tex, 10, 10, 2);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    std::string s = R"(
Exsuls experimentum in azureus cubiculum!
Purpose, dogma and an apostolic underworld.
To the chilled pickles add leek, rhubarb, coffee and sour blood oranges.
God, taste me lubber, ye sunny jack!
Dozens of collision courses will be lost in courages like hypnosis in minerals.
)";
    s = s.substr(1, s.size() - 1);

    auto bounds = font->bounds(1, s);
    font->draw(std::floor(window->w() / 2) - std::floor(bounds.x / 2),
               std::floor(window->h() / 2) - std::floor(bounds.y / 2),
               1, s);
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 720},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
