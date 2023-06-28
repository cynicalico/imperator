#include "baphy/baphy.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"

auto DATA = std::filesystem::path(__FILE__).parent_path() / "data";

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::ShaderMgr> shaders{nullptr};
  std::shared_ptr<baphy::Shader> basic_shader{nullptr};

  GLuint vao;
  GLuint vbo;

  void initialize() override {
    shaders = module_mgr->get<baphy::ShaderMgr>();
    basic_shader = shaders->compile(*baphy::ShaderSrc::parse(DATA / "shader" / "basic.shader"));

    float vertices[] = {
        10.0, 10.0, 0.0,
        10.0, 110.0, 0.0,
        110.0, 110.0, 0.0
    };

    gfx->gl->GenVertexArrays(1, &vao);
    gfx->gl->BindVertexArray(vao);

    gfx->gl->GenBuffers(1, &vbo);
    gfx->gl->BindBuffer(GL_ARRAY_BUFFER, vbo);
    gfx->gl->BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    gfx->gl->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    gfx->gl->EnableVertexAttribArray(0);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);
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
  .flags = baphy::WindowFlags::centered
)
