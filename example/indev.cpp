#include "baphy/baphy.hpp"
#include "baphy/gfx/gl/shader.hpp"

class Indev : public baphy::Application {
public:
  std::unique_ptr<baphy::Shader> shader;
  GLuint vao, vbo;

  std::vector<float> vertices;
  float radius{100.0f};
  float theta{0.0f};

  double fps{0.0};

  void initialize() override;

  void update(double dt) override;

  void draw() override;

  void regen_vertices();
};

void Indev::initialize() {
  auto cwd = std::filesystem::current_path();
  auto shader_src = baphy::ShaderSrc::parse(
    cwd / "res" / "shader" / "basic.glsl");
  shader = std::make_unique<baphy::Shader>(*gfx, *shader_src);

  regen_vertices();

  gfx->gl.GenVertexArrays(1, &vao);
  gfx->gl.BindVertexArray(vao);

  gfx->gl.GenBuffers(1, &vbo);
  gfx->gl.BindBuffer(GL_ARRAY_BUFFER, vbo);
  gfx->gl.BufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

  gfx->gl.VertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
  gfx->gl.EnableVertexAttribArray(0);
}

void Indev::update(double dt) {
  theta += 20.0f * dt;
  regen_vertices();

  gfx->gl.BindBuffer(GL_ARRAY_BUFFER, vbo);
  gfx->gl.BufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

  fps = 1.0 / dt;
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  dear->new_frame();

  shader->use();
  gfx->gl.BindVertexArray(vao);
  gfx->gl.DrawArrays(GL_TRIANGLES, 0, 3);

  if (ImGui::Begin("FPS")) {
    ImGui::Text("%s", fmt::format("{:.2f} fps", fps).c_str());
  }
  ImGui::End();

  dear->render();
}

void Indev::regen_vertices() {
  float c_x = window->w() / 2.0f;
  float c_y = window->h() / 2.0f;

  float v1_x = baphy::normalize(c_x + radius * std::sin(glm::radians(theta)), 0, window->w(), -1.0, 1.0);
  float v1_y = baphy::normalize(c_y + radius * std::cos(glm::radians(theta)), 0, window->h(), -1.0, 1.0);

  float v2_x = baphy::normalize(c_x + radius * std::sin(glm::radians(120 + theta)), 0, window->w(), -1.0, 1.0);
  float v2_y = baphy::normalize(c_y + radius * std::cos(glm::radians(120 + theta)), 0, window->h(), -1.0, 1.0);

  float v3_x = baphy::normalize(c_x + radius * std::sin(glm::radians(240 + theta)), 0, window->w(), -1.0, 1.0);
  float v3_y = baphy::normalize(c_y + radius * std::cos(glm::radians(240 + theta)), 0, window->h(), -1.0, 1.0);

  vertices = {
    v1_x, v1_y, 0.0f,
    v2_x, v2_y, 0.0f,
    v3_x, v3_y, 0.0f
  };
}

int main(int, char*[]) {
  auto e = baphy::Engine();
  e.run_application<Indev>(baphy::InitializeParams{
    .title = "Indev",
    .size = {500, 500},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
  });
}
