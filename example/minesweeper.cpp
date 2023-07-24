#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Minesweeper {
public:

private:
};

class MinesweeperApp : public baphy::Application {
public:
  std::unique_ptr<baphy::Spritesheet> ss{};

  void initialize() override {
    ss = std::make_unique<baphy::Spritesheet>(
        *textures,
        HERE / "res" / "img" / "minesweeper-sheet.png",
        HERE / "res" / "img" / "minesweeper-sheet.json",
        true
    );

    window->set_size(500, 500);
    window->center();
    window->show();
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    ss->draw("title", input->mouse_x(), input->mouse_y(), 4);
  }
};

BAPHY_RUN(MinesweeperApp,
    .title = "Minesweeper",
    .flags = baphy::WindowFlags::hidden | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
