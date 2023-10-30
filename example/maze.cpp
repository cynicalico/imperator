#include "baphy/baphy.hpp"

const auto HERE        = std::filesystem::path(__FILE__).parent_path();
const auto ICON_DIR    = HERE / "res" / "img" / "icon" / "snake";
const auto SSHEET_PATH = HERE / "res" / "img" / "colored_tilemap_packed.png";
const auto SSHEET_SPEC = HERE / "res" / "img" / "colored_tilemap_packed.json";

class Maze : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> ssheet_tex;
  std::unique_ptr<baphy::Spritesheet> ssheet;

  void initialize() override;
  void update(double dt) override;
  void draw() override;
};

void Maze::initialize() {
  window->set_icon_dir(ICON_DIR);

  ssheet_tex = textures->load(SSHEET_PATH, true);
  ssheet = std::make_unique<baphy::Spritesheet>(ssheet_tex, SSHEET_SPEC);

  ssheet->set_scale(2);
}

void Maze::update(double dt) {
  if (input->pressed("escape"))
    window->set_should_close(true);
}

void Maze::draw() {
  gfx->clear(baphy::rgb("black"));

  primitives->line(0, 0, input->mouse_x(), input->mouse_y(), baphy::rgb("red"));

  primitives->draw_rect(0, 0, input->mouse_x(), input->mouse_y(), baphy::rgb("green"));
}

BAPHY_RUN(Maze,
    .title = "Maze",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered
)