#ifndef EXAMPLE_INDEV_HPP
#define EXAMPLE_INDEV_HPP

#include "imperator/imperator_gl_wrappers.hpp"

class PrimitiveBatcher {
public:
  PrimitiveBatcher(imp::GfxContext& gfx);

  void clear();

  void triangle(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float r, float g, float b
  );

  void draw(const glm::mat4& mvp);

private:
  std::unique_ptr<imp::Shader> shader;
  std::unique_ptr<imp::VertexArray> vao;
  std::unique_ptr<imp::IVBuffer> ibo;
  std::unique_ptr<imp::FVBuffer> vbo;
  std::unique_ptr<imp::FVBuffer> mbo;
};

inline PrimitiveBatcher::PrimitiveBatcher(imp::GfxContext& gfx) {
  const auto cwd = std::filesystem::current_path();
  const auto shader_src = imp::ShaderSrc::parse(
    cwd / "res" / "shader" / "basic.glsl");
  shader = std::make_unique<imp::Shader>(gfx, *shader_src);

  ibo = std::make_unique<imp::IVBuffer>(gfx, 4, false, imp::BufTarget::draw_indirect, imp::BufUsage::stream_draw);
  vbo = std::make_unique<imp::FVBuffer>(gfx, 3, false, imp::BufTarget::array, imp::BufUsage::stream_draw);
  mbo = std::make_unique<imp::FVBuffer>(gfx, 6, false, imp::BufTarget::array, imp::BufUsage::stream_draw);

  vao = std::make_unique<imp::VertexArray>(gfx);
  vao->attrib(*shader, imp::BufTarget::array, *vbo, "in_pos:3f");
  vao->attrib(*shader, imp::BufTarget::array, *mbo, "in_color:3f:i1 in_trans:3f:i1");
}

inline void PrimitiveBatcher::clear() {
  ibo->clear();
  vbo->clear();
  mbo->clear();
}

inline void PrimitiveBatcher::triangle(
  float x0, float y0,
  float x1, float y1,
  float x2, float y2,
  float r, float g, float b
) {
  int n = ibo->size() / 4;
  ibo->add({3, 1, n * 3, n});
  vbo->add({x0, y0, 0.0f, x1, y1, 0.0f, x2, y2, 0.0f});
  mbo->add({r, g, b, 0.0f, 0.0f, 0.0f});
}

inline void PrimitiveBatcher::draw(const glm::mat4& mvp) {
  ibo->sync();
  vbo->sync();
  mbo->sync();

  if (ibo->size() > 0) {
    shader->use();
    shader->uniform_mat4f("mvp", mvp);
    ibo->bind(imp::BufTarget::draw_indirect);
    vao->multi_draw_arrays_indirect(imp::DrawMode::triangles, ibo->size() / 4, sizeof(float) * 4);
  }
}

#endif//EXAMPLE_INDEV_HPP
