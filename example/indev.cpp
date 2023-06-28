#include "baphy/baphy.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/gfx/internal/gl/static_buffer.hpp"

auto DATA = std::filesystem::path(__FILE__).parent_path() / "data";

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::ShaderMgr> shaders{nullptr};
  std::shared_ptr<baphy::Shader> basic_shader{nullptr};

  GLuint vao;
  std::unique_ptr<baphy::FSBuffer> vbo{nullptr};

  void initialize() override {
    shaders = module_mgr->get<baphy::ShaderMgr>();
    basic_shader = shaders->compile(*baphy::ShaderSrc::parse(DATA / "shader" / "basic.shader"));

    gfx->gl->GenVertexArrays(1, &vao);
    gfx->gl->BindVertexArray(vao);

    vbo = std::make_unique<baphy::FSBuffer>(
        gfx,
        baphy::BufTarget::array,
        baphy::BufUsage::static_draw,
        std::vector {
            window->w() - 10.0f, 10.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            window->w() - 10.0f, window->h() - 10.0f, 0.0f
        }
    );
    vbo->bind(baphy::BufTarget::array);

    gfx->gl->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    gfx->gl->EnableVertexAttribArray(0);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    vbo->write_sub(3, {static_cast<float>(input->mouse_x()), static_cast<float>(input->mouse_y()), 0.0f});
  }

  void draw() override {
    gfx->clear(baphy::rgb(0.06 * 255, 0.06 * 255, 0.06 * 255));

    basic_shader->use();
    basic_shader->uniform_mat4f("mvp", gfx->ortho_projection());

    gfx->gl->BindVertexArray(vao);
    gfx->gl->DrawArrays(GL_TRIANGLES, 0, 3);
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 800},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
