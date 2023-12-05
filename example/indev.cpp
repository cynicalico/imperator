#include "indev.hpp"

class Indev : public imp::Application {
public:
  std::unique_ptr<imp::Shader> shader;
  GLuint vao;
  GLuint vbo;

  std::vector<float> vertices;

  imp::FrameCounter fps{};

  void initialize() override;

  void update(double dt) override;

  void draw() override;
};

void Indev::initialize() {
  const auto cwd = std::filesystem::current_path();
  const auto shader_src = imp::ShaderSrc::parse(
    cwd / "res" / "shader" / "basic.glsl");
  shader = std::make_unique<imp::Shader>(*gfx, *shader_src);

  auto add_vertex = [&](
    float x, float y, float z,
    float rot_x, float rot_y, float rot_a,
    float r, float g, float b, float a
  ) {
    vertices.emplace_back(x);
    vertices.emplace_back(y);
    vertices.emplace_back(z);
    vertices.emplace_back(rot_x);
    vertices.emplace_back(rot_y);
    vertices.emplace_back(rot_a);
    vertices.emplace_back(r);
    vertices.emplace_back(g);
    vertices.emplace_back(b);
    vertices.emplace_back(a);
    IMPERATOR_LOG_INFO("{} {} {} {} {} {} {} {} {} {}", x, y, z, rot_x, rot_y, rot_a, r, g, b, a);
  };

  std::vector<glm::dvec2> points = {{125, 125}, {150, 150}};
  std::vector<glm::dvec4> colors = {{255, 255, 255, 255}, {255, 255, 255, 255}};
  std::vector<double> weights = {1, 1};
  const imp::PolylineOpt opts{};
  const imp::PolylineOptInternal opts_internal{imp::PolylineOptInternal::const_color | imp::PolylineOptInternal::const_weight};
  const auto vertices_to_add = imp::polyline(points, colors, weights, opts, opts_internal);
  for (const auto &v: vertices_to_add)
    add_vertex(v.x, v.y, 0, 0, 0, 0, v.r, v.g, v.b, v.a);

  gfx->gl.GenVertexArrays(1, &vao);
  gfx->gl.BindVertexArray(vao);

  gfx->gl.GenBuffers(1, &vbo);
  gfx->gl.BindBuffer(GL_ARRAY_BUFFER, vbo);
  gfx->gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

  gfx->gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)0);
  gfx->gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(sizeof(float) * 3));
  gfx->gl.VertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(sizeof(float) * 6));

  gfx->gl.EnableVertexAttribArray(0);
  gfx->gl.EnableVertexAttribArray(1);
  gfx->gl.EnableVertexAttribArray(2);
}

void Indev::update(double dt) {
  fps.update();
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gfx->gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gfx->gl.Enable(GL_BLEND);

  shader->use();
  shader->uniform_mat4f("mvp", window->projection_matrix());
  gfx->gl.BindVertexArray(vao);
  gfx->gl.DrawArrays(GL_TRIANGLES, 0, vertices.size() / 10);

  gfx->gl.Disable(GL_BLEND);

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
    .size = {500, 500},
    .flags = imp::WindowFlags::centered // | imp::WindowFlags::vsync
  });
}

// auto add_vertex = [&](
//   std::int16_t x, std::int16_t y,
//   std::int16_t z, std::int16_t rot_a,
//   std::int16_t rot_x, std::int16_t rot_y,
//   std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a
// ) {
//   std::uint32_t xy = ((y << 16) & 0xffff0000) | (x & 0xffff);
//   std::uint32_t za = ((rot_a << 16) & 0xffff0000) | (z & 0xffff);
//   std::uint32_t rot_xy = ((rot_y << 16) & 0xffff0000) | (rot_x & 0xffff);
//   std::uint32_t rgba = (a << 24) | (b << 16) | (g << 8) | r;
//   vertices.push_back(xy);
//   vertices.push_back(za);
//   vertices.push_back(rot_xy);
//   vertices.push_back(rgba);
// };
//
// std::vector<glm::dvec2> points = {{100, 100}, {150, 150}};
// std::vector<glm::dvec4> colors = {{255, 255, 255, 255}, {255, 255, 255, 255}};
// std::vector<double> weights = {1, 1};
// const imp::PolylineOpt opts{};
// const imp::PolylineOptInternal opts_internal{imp::PolylineOptInternal::const_color | imp::PolylineOptInternal::const_weight};
// const auto vertices = imp::polyline(points, colors, weights, opts, opts_internal);
// for (const auto &v: vertices)
//   add_vertex(v.x, v.y, 0, 0, 0, 0, v.r, v.g, v.b, v.a);
//
// gfx->gl.GenVertexArrays(1, &vao);
// gfx->gl.BindVertexArray(vao);
//
// gfx->gl.GenBuffers(1, &vbo);
// gfx->gl.BindBuffer(GL_ARRAY_BUFFER, vbo);
// gfx->gl.BufferData(GL_ARRAY_BUFFER, sizeof(std::uint32_t) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
//
// gfx->gl.VertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, sizeof(std::uint32_t) * 4, (void*)0);
// gfx->gl.VertexAttribPointer(1, 3, GL_SHORT, GL_FALSE, sizeof(std::uint32_t) * 4, (void*)(sizeof(short) * 3));
// gfx->gl.VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(std::uint32_t) * 4, (void*)(sizeof(short) * 6));
//
// gfx->gl.EnableVertexAttribArray(0);
// gfx->gl.EnableVertexAttribArray(1);
// gfx->gl.EnableVertexAttribArray(2);