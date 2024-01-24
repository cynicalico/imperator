#include "imp/gfx/module/gfx_2d.hpp"

namespace imp {
std::once_flag Gfx2D::created_required_modules_;

Gfx2D::Gfx2D(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
  std::call_once(created_required_modules_, [&] {
    module_mgr.lock()->create<Batcher>();
  });

  batcher = module_mgr.lock()->get<Batcher>();
  ctx = module_mgr.lock()->get<GfxContext>();
}

void Gfx2D::clear(const Color& color, const ClearBit& mask) {
  const auto gl_color = color.gl_color();
  ctx->gl.ClearColor(gl_color.r, gl_color.g, gl_color.b, gl_color.a);
  ctx->gl.Clear(unwrap(mask));
}

void Gfx2D::point(glm::vec2 xy, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {xy.x, xy.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::points, vdata);
  } else {
    batcher->add_opaque(DrawMode::points, vdata);
  }
}

void Gfx2D::line(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 rcenter, const float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    p0.x, p0.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p1.x, p1.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::lines, vdata);
  } else {
    batcher->add_opaque(DrawMode::lines, vdata);
  }
}

void Gfx2D::line(const glm::vec2 p0, const glm::vec2 p1, float angle, const Color& c) {
  const glm::vec2 midpoint = {(p0.x + p1.x) / 2.0f, (p0.y + p1.y) / 2.0f};
  line(p0, p1, midpoint, angle, c);
}

void Gfx2D::line(const glm::vec2 p0, const glm::vec2 p1, const Color& c) {
  line(p0, p1, {0, 0}, 0, c);
}

void Gfx2D::tri(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 rcenter, float angle,
                const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    p0.x, p0.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p1.x, p1.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p2.x, p2.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::triangles, vdata);
  } else {
    batcher->add_opaque(DrawMode::triangles, vdata);
  }
}

void Gfx2D::tri(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, float angle, const Color& c) {
  const glm::vec2 centroid = {(p0.x + p1.x + p2.x) / 3.0f, (p0.y + p1.y + p2.y) / 3.0f};
  tri(p0, p1, p2, centroid, angle, c);
}

void Gfx2D::tri(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, const Color& c) {
  tri(p0, p1, p2, {0, 0}, 0, c);
}

void Gfx2D::rect(const glm::vec2 xy, const glm::vec2 size, const glm::vec2 rcenter, float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    xy.x, xy.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x, xy.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x, xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::triangles, vdata);
  } else {
    batcher->add_opaque(DrawMode::triangles, vdata);
  }
}

void Gfx2D::rect(const glm::vec2 xy, const glm::vec2 size, float angle, const Color& c) {
  const glm::vec2 center = {xy.x + size.x / 2.0f, xy.y + size.y / 2.0f};
  rect(xy, size, center, angle, c);
}

void Gfx2D::rect(const glm::vec2 xy, const glm::vec2 size, const Color& c) {
  rect(xy, size, {0, 0}, 0, c);
}
} // namespace imp
