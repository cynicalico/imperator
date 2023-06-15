#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  void initialize() override {}

  void update(double dt) override {}

  void draw() override {
    gfx->clear(baphy::rgb(0x222244));
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {500, 500},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered
)
