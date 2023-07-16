#include "baphy/baphy.hpp"
#include "baphy/gfx/internal/gl/framebuffer.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::unique_ptr<baphy::Framebuffer> fb{nullptr};
  bool first{true};

  void initialize() override {
    fb = baphy::FramebufferBuilder(*gfx, window->w(), window->h())
      .texture(baphy::TexFormat::rgba32f)
      .renderbuffer(baphy::RBufFormat::d32f)
      .check_complete();
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);
  }

  void draw() override {
    gfx->clear(baphy::rgb("red"));

    fb->bind();
    if (first) {
      gfx->clear(baphy::rgb("black"));
      first = false;
    }
    primitives->tri_equilateral(input->mouse_x(), input->mouse_y(), 100, baphy::rgb("lime"));
    gfx->flush_draw_calls();
    fb->unbind();

    fb->copy_to_default_framebuffer();
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 960},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
