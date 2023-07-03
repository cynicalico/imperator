#include "baphy/baphy.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/gfx/internal/gl/static_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"

auto DATA = std::filesystem::path(__FILE__).parent_path() / "data";

class Indev : public baphy::Application {
public:
  baphy::HSV tri_color{baphy::hsv(0.0, 1.0, 1.0)};

  std::shared_ptr<baphy::ThreadPool> pool{nullptr};

  std::shared_ptr<baphy::ShaderMgr> shaders{nullptr};
  std::shared_ptr<baphy::Shader> basic_shader{nullptr};

  std::unique_ptr<baphy::VertexArray> vao{nullptr};
  std::unique_ptr<baphy::FSBuffer> vbo{nullptr};

  void initialize() override {
    pool = module_mgr->get<baphy::ThreadPool>();

    shaders = module_mgr->get<baphy::ShaderMgr>();
    basic_shader = shaders->compile(*baphy::ShaderSrc::parse(DATA / "shader" / "basic.shader"));

    vao = std::make_unique<baphy::VertexArray>(gfx);

    vbo = std::make_unique<baphy::FSBuffer>(
        gfx,
        baphy::BufTarget::array,
        baphy::BufUsage::static_draw,
        std::vector {
            window->w() - 10.0f, 10.0f,               0.0f,   1.0f, 1.0f, 1.0f,
            0.0f,                0.0f,                0.0f,   1.0f, 1.0f, 1.0f,
            window->w() - 10.0f, window->h() - 10.0f, 0.0f,   1.0f, 1.0f, 1.0f
        }
    );
    vao->attrib(*basic_shader, *vbo, "pos:3f color:3f");
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("1"))
      pool->add_job("cancel_me", [&](auto wait) {
        BAPHY_LOG_INFO("3...");
        wait(1.0);
        BAPHY_LOG_INFO("2...");
        wait(1.0);
        BAPHY_LOG_INFO("1...");
        wait(1.0);
        BAPHY_LOG_INFO("Done!");
      });

    if (input->pressed("2"))
      pool->cancel_job("cancel_me");

    vbo->write_sub(6, {static_cast<float>(input->mouse_x()), static_cast<float>(input->mouse_y()), 0.0f});

    tri_color.h = std::fmod(tri_color.h + (dt * 90.0), 360.0);
    auto c = tri_color.to_rgb();
    auto cv = std::vector {c.r / 255.0f, c.g / 255.0f, c.b / 255.0f};
    vbo->write_sub(3, cv);
    vbo->write_sub(9, cv);
    vbo->write_sub(15, cv);
  }

  void draw() override {
    gfx->clear(baphy::rgb_f(0.06, 0.06, 0.06));

    basic_shader->use();
    basic_shader->uniform_mat4f("mvp", gfx->ortho_projection());

    vao->draw_arrays(baphy::DrawMode::triangles, 3);
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 800},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
