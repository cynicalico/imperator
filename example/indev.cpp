#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Batcher> batcher{nullptr};
  std::shared_ptr<baphy::Texture> tex{nullptr};

  void initialize() override {
    batcher = module_mgr->get<baphy::Batcher>();

    tex = textures->load(HERE / "res" / "img" / "tile005.png", true);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgba(0x00000000));

    primitives->tri_equilateral(window->w() / 2, window->h() / 2, 100, 90, baphy::rgb("red"));

    tex->draw(input->mouse_x(), input->mouse_y());
    tex->draw(input->mouse_x() + 20, input->mouse_y());
    tex->draw(input->mouse_x() + 40, input->mouse_y());
    tex->draw(input->mouse_x() + 60, input->mouse_y());
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {500, 500},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
