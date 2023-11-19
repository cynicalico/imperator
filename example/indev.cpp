#include "baphy/baphy.hpp"
#include "baphy/baphy_gl_wrappers.hpp"
#include "glm/ext/matrix_clip_space.hpp"

class Indev : public baphy::Application {
public:
  std::unique_ptr<baphy::Shader> shader;
  glm::mat4 mvp;

  std::unique_ptr<baphy::FVBuffer> vbo;
  std::unique_ptr<baphy::VertexArray> vao;

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

  mvp = glm::ortho(0.0f, (float)window->w(), (float)window->h(), 0.0f);

  vbo = std::make_unique<baphy::FVBuffer>(*gfx, 18, false, baphy::BufTarget::array, baphy::BufUsage::stream_draw);

  vao = std::make_unique<baphy::VertexArray>(*gfx);
  vao->attrib(*shader, *vbo, "in_pos:3f in_color:3f");
}

void Indev::update(double dt) {
  theta += 20.0f * dt;

  fps.update();
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  vbo->clear();
  for (std::size_t i = 0; i < 375000; ++i) {
    auto c_x = baphy::rnd::get<float>(0, window->w());
    auto c_y = baphy::rnd::get<float>(0, window->h());
    auto v1_x = baphy::rnd::get<float>(c_x - radius, c_x + radius);
    auto v1_y = baphy::rnd::get<float>(c_y - radius, c_y + radius);
    auto v2_x = baphy::rnd::get<float>(c_x - radius, c_x + radius);
    auto v2_y = baphy::rnd::get<float>(c_y - radius, c_y + radius);
    auto v3_x = baphy::rnd::get<float>(c_x - radius, c_x + radius);
    auto v3_y = baphy::rnd::get<float>(c_y - radius, c_y + radius);
    auto r = baphy::rnd::get<float>();
    auto g = baphy::rnd::get<float>();
    auto b = baphy::rnd::get<float>();
    vbo->add({
      v1_x, v1_y, 0.0f,  r, g, b,
      v2_x, v2_y, 0.0f,  r, g, b,
      v3_x, v3_y, 0.0f,  r, g, b,
    });
  }
  vbo->sync();

  shader->use();
  shader->uniform_mat4f("mvp", mvp);
  vao->draw_arrays(baphy::DrawMode::triangles, vbo->size() / 6);

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
    .flags = baphy::WindowFlags::centered // | baphy::WindowFlags::vsync
  });
}
