#include "baphy/baphy.hpp"
#include "baphy/gfx/internal/gl/shader.hpp"
#include <filesystem>

auto DATA = std::filesystem::path(__FILE__).parent_path() / "data";

class Indev : public baphy::Application {
public:
  void initialize() override {
    auto ss_o = baphy::ShaderSrc::parse(DATA / "shader" / "basic.shader");
    if (ss_o)
      baphy::Shader(gfx, *ss_o);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x111122));
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 800},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered
)
