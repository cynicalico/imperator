#include "baphy/baphy.hpp"
#include "baphy/baphy_gl_wrappers.hpp"

class Indev : public baphy::Application {
public:
  std::unique_ptr<baphy::Shader> shader;
  std::unique_ptr<baphy::FSBuffer> vbo;
  std::unique_ptr<baphy::VertexArray> vao;

  std::vector<float> vertices;
  float radius{100.0f};
  float theta{0.0f};

  baphy::FrameCounter fps{};

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
  vbo = std::make_unique<baphy::FSBuffer>(*gfx, baphy::BufTarget::array, baphy::BufUsage::stream_draw, vertices);

  vao = std::make_unique<baphy::VertexArray>(*gfx);
  vao->attrib(*shader, *vbo, "in_pos:3f");
}

void Indev::update(double dt) {
  theta += 20.0f * dt;

  regen_vertices();
  vbo->write_sub(0, vertices);

  fps.update();
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  dear->new_frame();

  shader->use();
  vao->draw_arrays(baphy::DrawMode::triangles, 3);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
  ImGui::SetNextWindowPos({0, 0});
  if (ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%s", fmt::format("{:.2f} fps", fps.fps()).c_str());
    ImGui::Text("%s", fmt::format("{:.2f} MB", baphy::memusage_mb()).c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar(2);

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
