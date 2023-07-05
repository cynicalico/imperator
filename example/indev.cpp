#include "baphy/baphy.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/gfx/internal/gl/static_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"

auto DATA = std::filesystem::path(__FILE__).parent_path() / "data";

class Indev : public baphy::Application {
public:
  float x{0.0f};
  float y{0.0f};
  float w{50.0f};
  float h{50.0f};
  baphy::HSV color{baphy::hsv(0.0, 1.0, 1.0)};

  std::shared_ptr<baphy::ShaderMgr> shaders{nullptr};
  std::shared_ptr<baphy::Shader> basic_shader{nullptr};

  std::unique_ptr<baphy::VertexArray> vao{nullptr};
  std::unique_ptr<baphy::FSBuffer> vbo{nullptr};

  void initialize() override {
    shaders = module_mgr->get<baphy::ShaderMgr>();
    basic_shader = shaders->compile(*baphy::ShaderSrc::parse(DATA / "shader" / "basic.shader"));

    vao = std::make_unique<baphy::VertexArray>(gfx);

    vbo = std::make_unique<baphy::FSBuffer>(
        gfx, baphy::BufTarget::array, baphy::BufUsage::stream_draw, std::vector(3 * 6, 0.0f));
    vao->attrib(*basic_shader, *vbo, "pos:3f");

    x = window->w() / 2.0f;
    y = window->h() / 2.0f;
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("mb_left")) {
      auto f_mx = static_cast<float>(input->mouse_x());
      auto f_my = static_cast<float>(input->mouse_y());
      tween->begin("x", x, f_mx, 2.0, baphy::Easing::in_bounce, [&](auto v) { x = v; });
      tween->begin("y", y, f_my, 2.0, baphy::Easing::out_bounce, [&](auto v) { y = v; });
    }

    vbo->write_sub(0, {
        x - (w / 2.0f), y - (h / 2.0f), 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), 0.0f,
        x - (w / 2.0f), y + (h / 2.0f), 0.0f,
        x + (w / 2.0f), y + (h / 2.0f), 0.0f,
        x + (w / 2.0f), y - (h / 2.0f), 0.0f,
    });

    color.h = std::fmod(color.h + (90.0f * dt), 360.0f);
  }

  void draw() override {
    gfx->clear(baphy::rgb_f(0.06, 0.06, 0.06));

    basic_shader->use();
    basic_shader->uniform_mat4f("mvp", gfx->ortho_projection());
    basic_shader->uniform_3f("color", color.to_rgb().vec3());

    vao->draw_arrays(baphy::DrawMode::triangles, 6);
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 800},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
