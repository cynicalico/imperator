#include "baphy/baphy.hpp"
#include "baphy/gfx/surface.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::ShaderMgr> shaders{nullptr};
  std::unique_ptr<baphy::Surface> surf{nullptr};

  void initialize() override {
    shaders = module_mgr->get<baphy::ShaderMgr>();
    surf = std::make_unique<baphy::Surface>(gfx, shaders, 150, 150);
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb("red"));

    surf->draw_on([&] {
      gfx->clear(baphy::rgb("blue"));
      primitives->tri_equilateral(75, 75, 50, baphy::rgb("lime"));
    });

    surf->draw(input->mouse_x(), input->mouse_y());
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 960},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
