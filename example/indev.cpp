#include "imperator/imperator.hpp"
#include "indev.hpp"

class Indev : public imp::Application {
public:
  std::unique_ptr<PrimitiveBatcher> primitives;
  imp::FrameCounter fps{};

  void initialize() override;

  void update(double dt) override;

  void draw() override;
};

void Indev::initialize() {
  primitives = std::make_unique<PrimitiveBatcher>(*gfx);
}

void Indev::update(double dt) {
  fps.update();

  primitives->clear();
  for (std::size_t N = 0; N < 2'000'000; ++N) {
    float x = imp::rnd::get<float>(0, window->w());
    float y = imp::rnd::get<float>(0, window->h());
    float x0 = imp::rnd::get<float>(x - 50, x + 50);
    float y0 = imp::rnd::get<float>(y - 50, y + 50);
    float x1 = imp::rnd::get<float>(x - 50, x + 50);
    float y1 = imp::rnd::get<float>(y - 50, y + 50);
    float x2 = imp::rnd::get<float>(x - 50, x + 50);
    float y2 = imp::rnd::get<float>(y - 50, y + 50);
    float r = imp::rnd::get<float>();
    float g = imp::rnd::get<float>();
    float b = imp::rnd::get<float>();
    primitives->triangle(x0, y0, x1, y1, x2, y2, r, g, b);
  }
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  primitives->draw(window->projection_matrix());

  dear->new_frame();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
  ImGui::SetNextWindowPos({0, 0});
  if (ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%s", fmt::format("{:.2f} fps", fps.fps()).c_str());
    ImGui::Text("%s", fmt::format("{:.2f} MB", imp::memusage_mb()).c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar(2);

  dear->render();
}

int main(int, char*[]) {
  auto e = imp::Engine();
  e.run_application<Indev>(imp::InitializeParams{
    .title = "Indev",
    .size = {1280, 720},
    .flags = imp::WindowFlags::centered // | imp::WindowFlags::vsync
  });
}

// #include "imperator/imperator.hpp"
// #include "imperator/imperator_gl_wrappers.hpp"
// #include "glm/ext/matrix_clip_space.hpp"
//
// class Indev : public imp::Application {
// public:
//   std::unique_ptr<imp::Shader> shader;
//   glm::mat4 mvp;
//
//   std::unique_ptr<imp::FVBuffer> vbo;
//   std::unique_ptr<imp::VertexArray> vao;
//
//   std::vector<float> vertices;
//   float radius{20.0f};
//   float theta{0.0f};
//
//   imp::FrameCounter fps{};
//
//   void initialize() override;
//
//   void update(double dt) override;
//
//   void draw() override;
// };
//
// void Indev::initialize() {
//   const auto cwd = std::filesystem::current_path();
//   const auto shader_src = imp::ShaderSrc::parse(
//     cwd / "res" / "shader" / "basic.glsl");
//   shader = std::make_unique<imp::Shader>(*gfx, *shader_src);
//
//   mvp = glm::ortho(0.0f, (float)window->w(), (float)window->h(), 0.0f);
//
//   vbo = std::make_unique<imp::FVBuffer>(*gfx, 18, false, imp::BufTarget::array, imp::BufUsage::stream_draw);
//
//   vao = std::make_unique<imp::VertexArray>(*gfx);
//   vao->attrib(*shader, *vbo, "in_pos:3f in_color:3f");
// }
//
// void Indev::update(double dt) {
//   theta += 20.0f * dt;
//
//   fps.update();
// }
//
// void Indev::draw() {
//   gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//   gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//   vbo->clear();
//   for (std::size_t i = 0; i < 2'000'000; ++i) {
//     auto c_x = imp::rnd::get<float>(0, window->w());
//     auto c_y = imp::rnd::get<float>(0, window->h());
//     auto v1_x = imp::rnd::get<float>(c_x - radius, c_x + radius);
//     auto v1_y = imp::rnd::get<float>(c_y - radius, c_y + radius);
//     auto v2_x = imp::rnd::get<float>(c_x - radius, c_x + radius);
//     auto v2_y = imp::rnd::get<float>(c_y - radius, c_y + radius);
//     auto v3_x = imp::rnd::get<float>(c_x - radius, c_x + radius);
//     auto v3_y = imp::rnd::get<float>(c_y - radius, c_y + radius);
//     auto r = imp::rnd::get<float>();
//     auto g = imp::rnd::get<float>();
//     auto b = imp::rnd::get<float>();
//     vbo->add({
//       v1_x, v1_y, 0.0f,  r, g, b,
//       v2_x, v2_y, 0.0f,  r, g, b,
//       v3_x, v3_y, 0.0f,  r, g, b,
//     });
//   }
//   vbo->sync();
//
//   shader->use();
//   shader->uniform_mat4f("mvp", mvp);
//   vao->draw_arrays(imp::DrawMode::triangles, vbo->size() / 6);
//
//   dear->new_frame();
//
//   ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
//   ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
//   ImGui::SetNextWindowPos({0, 0});
//   if (ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
//     ImGui::Text("%s", fmt::format("{:.2f} fps", fps.fps()).c_str());
//     ImGui::Text("%s", fmt::format("{:.2f} MB", imp::memusage_mb()).c_str());
//   }
//   ImGui::End();
//   ImGui::PopStyleVar(2);
//
//   dear->render();
// }
//
// int main(int, char*[]) {
//   auto e = imp::Engine();
//   e.run_application<Indev>(imp::InitializeParams{
//     .title = "Indev",
//     .size = {1280, 720},
//     .flags = imp::WindowFlags::centered // | imp::WindowFlags::vsync
//   });
// }