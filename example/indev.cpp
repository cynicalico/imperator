#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Batcher> batcher{nullptr};

  void initialize() override {
    batcher = module_mgr->get<baphy::Batcher>();
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb("white"));

    batcher->tri(200, 200, 300, 300, 200, 300, baphy::rgb("red"));
    batcher->tri(225, 250, 325, 350, 225, 350, baphy::rgb("lime"));

    batcher->tri(200, 400, 300, 500, 200, 500, baphy::rgba("red", 128));
    batcher->tri(225, 450, 325, 550, 225, 550, baphy::rgba("lime", 128));
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 600},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
