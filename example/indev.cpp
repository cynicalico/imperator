#include "baphy/baphy.hpp"
#include "baphy/baphy_gl_wrappers.hpp"

class Indev : public baphy::Application {
public:
  std::unique_ptr<baphy::Shader> shader;

  std::unique_ptr<baphy::VertexArray> vao;
  std::unique_ptr<baphy::IVBuffer> ibo;
  std::unique_ptr<baphy::FVBuffer> vbo;
  std::unique_ptr<baphy::FVBuffer> mbo;

  std::vector<float> vertices;
  float radius{20.0f};
  float theta{0.0f};

  baphy::FrameCounter fps{};

  void initialize() override;

  void update(double dt) override;

  void draw() override;
};

void Indev::initialize() {
  const auto cwd = std::filesystem::current_path();
  const auto shader_src = baphy::ShaderSrc::parse(
    cwd / "res" / "shader" / "basic.glsl");
  shader = std::make_unique<baphy::Shader>(*gfx, *shader_src);

  ibo = std::make_unique<baphy::IVBuffer>(*gfx, 4, false,
    baphy::BufTarget::draw_indirect, baphy::BufUsage::stream_draw);
  ibo->add({
    3, 1, 0, 0,
    6, 2, 3, 1
  });
  ibo->sync();

  vbo = std::make_unique<baphy::FVBuffer>(*gfx, 3, false,
    baphy::BufTarget::array, baphy::BufUsage::stream_draw);
  vbo->add({
    100.0f, 100.0f, 0.0f,
    150.0f, 150.0f, 0.0f,
    100.0f, 150.0f, 0.0f,

    100.0f, 200.0f, 0.0f,
    200.0f, 200.0f, 0.0f,
    100.0f, 300.0f, 0.0f,
    100.0f, 300.0f, 0.0f,
    200.0f, 200.0f, 0.0f,
    200.0f, 300.0f, 0.0f,
  });
  vbo->sync();

  mbo = std::make_unique<baphy::FVBuffer>(*gfx, 6, false,
    baphy::BufTarget::array, baphy::BufUsage::stream_draw);
  mbo->add({
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 150.0f, 250.0f, glm::radians(45.0f),
  });
  mbo->sync();

  vao = std::make_unique<baphy::VertexArray>(*gfx);
  vao->attrib(*shader, baphy::BufTarget::array, *vbo, "in_pos:3f");
  vao->attrib(*shader, baphy::BufTarget::array, *mbo, "in_color:3f:i1 in_trans:3f:i1");
}

void Indev::update(double dt) {
  theta += 20.0f * dt;

  fps.update();
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader->use();
  shader->uniform_mat4f("mvp", window->projection_matrix());
  ibo->bind(baphy::BufTarget::draw_indirect);
  vao->multi_draw_arrays_indirect(baphy::DrawMode::triangles, ibo->size() / 4, sizeof(float) * 4);

  dear->new_frame();

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

int main(int, char*[]) {
  auto e = baphy::Engine();
  e.run_application<Indev>(baphy::InitializeParams{
    .title = "Indev",
    .size = {1280, 720},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
  });
}
