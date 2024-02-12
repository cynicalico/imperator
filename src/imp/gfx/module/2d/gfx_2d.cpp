#include "imp/gfx/module/2d/gfx_2d.hpp"

namespace imp {
std::once_flag Gfx2D::created_required_modules_;

Gfx2D::Gfx2D(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
  std::call_once(created_required_modules_, [&] {
    module_mgr.lock()->create<Batcher>();
  });

  batcher = module_mgr.lock()->get<Batcher>();
  ctx = module_mgr.lock()->get<GfxContext>();
  textures = module_mgr.lock()->get<TextureMgr>();
}

void Gfx2D::clear(const Color& color, const ClearBit& mask) {
  const auto gl_color = color.gl_color();
  ctx->gl.ClearColor(gl_color.r, gl_color.g, gl_color.b, gl_color.a);
  ctx->gl.Clear(unwrap(mask));
}

void Gfx2D::point(glm::vec2 xy, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {xy.x, xy.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a};
  const std::initializer_list<unsigned int> idata = {0};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::points, vdata, idata);
  } else {
    batcher->add_opaque(DrawMode::points, vdata, idata);
  }
}

void Gfx2D::line(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 rcenter, const float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    p0.x, p0.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p1.x, p1.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };
  const std::initializer_list<unsigned int> idata = {0, 1};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::lines, vdata, idata);
  } else {
    batcher->add_opaque(DrawMode::lines, vdata, idata);
  }
}

void Gfx2D::line(const glm::vec2 p0, const glm::vec2 p1, float angle, const Color& c) {
  const glm::vec2 midpoint = {(p0.x + p1.x) / 2.0f, (p0.y + p1.y) / 2.0f};
  line(p0, p1, midpoint, angle, c);
}

void Gfx2D::line(const glm::vec2 p0, const glm::vec2 p1, const Color& c) {
  line(p0, p1, {0, 0}, 0, c);
}

void Gfx2D::draw_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 rcenter, float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    p0.x, p0.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p1.x, p1.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p2.x, p2.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };
  const std::initializer_list<unsigned int> idata = {0, 1, 2};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::line_loop, vdata, idata, true);
  } else {
    batcher->add_opaque(DrawMode::line_loop, vdata, idata, true);
  }
}

void Gfx2D::draw_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float angle, const Color& c) {
  const glm::vec2 centroid = {(p0.x + p1.x + p2.x) / 3.0f, (p0.y + p1.y + p2.y) / 3.0f};
  draw_tri(p0, p1, p2, centroid, angle, c);
}

void Gfx2D::draw_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, const Color& c) {
  draw_tri(p0, p1, p2, {0, 0}, 0, c);
}

void Gfx2D::fill_tri(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 rcenter, float angle,
                     const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    p0.x, p0.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p1.x, p1.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    p2.x, p2.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };
  const std::initializer_list<unsigned int> idata = {0, 1, 2};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::triangles, vdata, idata);
  } else {
    batcher->add_opaque(DrawMode::triangles, vdata, idata);
  }
}

void Gfx2D::fill_tri(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, float angle, const Color& c) {
  const glm::vec2 centroid = {(p0.x + p1.x + p2.x) / 3.0f, (p0.y + p1.y + p2.y) / 3.0f};
  fill_tri(p0, p1, p2, centroid, angle, c);
}

void Gfx2D::fill_tri(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, const Color& c) {
  fill_tri(p0, p1, p2, {0, 0}, 0, c);
}

void Gfx2D::draw_rect(glm::vec2 xy, glm::vec2 size, glm::vec2 rcenter, float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    xy.x,          xy.y,          batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y,          batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x,          xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };
  const std::initializer_list<unsigned int> idata = {0, 1, 2, 3};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::line_loop, vdata, idata, true);
  } else {
    batcher->add_opaque(DrawMode::line_loop, vdata, idata, true);
  }
}

void Gfx2D::draw_rect(glm::vec2 xy, glm::vec2 size, float angle, const Color& c) {
  const glm::vec2 center = {xy.x + size.x / 2.0f, xy.y + size.y / 2.0f};
  draw_rect(xy, size, center, angle, c);
}

void Gfx2D::draw_rect(glm::vec2 xy, glm::vec2 size, const Color& c) {
  draw_rect(xy, size, {0, 0}, 0, c);
}

void Gfx2D::fill_rect(const glm::vec2 xy, const glm::vec2 size, const glm::vec2 rcenter, float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    xy.x,          xy.y,          batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y,          batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + size.x, xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x,          xy.y + size.y, batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, rcenter.x, rcenter.y, glm::radians(angle),
  };
  const std::initializer_list<unsigned int> idata = {0, 1, 2, 0, 2, 3};

  if (gl_c.a < 1.0) {
    batcher->add_trans(DrawMode::triangles, vdata, idata);
  } else {
    batcher->add_opaque(DrawMode::triangles, vdata, idata);
  }
}

void Gfx2D::fill_rect(const glm::vec2 xy, const glm::vec2 size, float angle, const Color& c) {
  const glm::vec2 center = {xy.x + size.x / 2.0f, xy.y + size.y / 2.0f};
  fill_rect(xy, size, center, angle, c);
}

void Gfx2D::fill_rect(const glm::vec2 xy, const glm::vec2 size, const Color& c) {
  fill_rect(xy, size, {0, 0}, 0, c);
}

void Gfx2D::draw_tex(const Texture& t, glm::vec2 xy, glm::vec2 rcenter, float angle, const Color& c) {
  const auto gl_c = c.gl_color();
  const std::initializer_list<float> vdata = {
    xy.x,         xy.y,         batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, 0.0f, 0.0f, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + t.w(), xy.y,         batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, 1.0f, 0.0f, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x + t.w(), xy.y + t.h(), batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, 1.0f, 1.0f, rcenter.x, rcenter.y, glm::radians(angle),
    xy.x,         xy.y + t.h(), batcher->z, gl_c.r, gl_c.g, gl_c.b, gl_c.a, 0.0f, 1.0f, rcenter.x, rcenter.y, glm::radians(angle),
  };
  const std::initializer_list<unsigned int> idata = {0, 1, 2, 0, 2, 3};

  if (!t.fully_opaque() || gl_c.a < 1.0) {
    batcher->add_trans_tex(t.id(), vdata, idata);
  } else {
    batcher->add_opaque_tex(t.id(), vdata, idata);
  }
}

void Gfx2D::draw_tex(const Texture& t, glm::vec2 xy, float angle, const Color& c) {
  const glm::vec2 center = {xy.x + t.w() / 2.0f, xy.y + t.h() / 2.0f};
  draw_tex(t, xy, center, angle, c);
}

void Gfx2D::draw_tex(const Texture& t, glm::vec2 xy, const Color& c) {
  draw_tex(t, xy, {0, 0}, 0, c);
}
} // namespace imp
