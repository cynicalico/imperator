#include "baphy/gfx/module/primitive_batcher.hpp"

namespace baphy {

void PrimitiveBatcher::fill_rect(float x, float y, float w, float h, float rx, float ry, float angle, const RGB &color) {
  auto cv = color.vec4();
  auto data = std::initializer_list<float>{
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x + w, y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),

      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x,     y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f) batcher->t_add_tri(data);
  else             batcher->o_add_tri(data);
}

void PrimitiveBatcher::fill_rect(float x, float y, float w, float h, float angle, const RGB &color) {
  fill_rect(x, y, w, h, x + (w / 2), y + (h / 2), angle, color);
}

void PrimitiveBatcher::fill_rect(float x, float y, float w, float h, const RGB &color) {
  fill_rect(x, y, w, h, 0, 0, 0, color);
}

void PrimitiveBatcher::draw_rect(float x, float y, float w, float h, float rx, float ry, float angle, const RGB &color) {
  auto cv = color.vec4();
  auto data = std::initializer_list<float>{
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x + w, y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),

      x + w, y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),

      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x,     y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),

      x,     y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f) batcher->t_add_line(data);
  else             batcher->o_add_line(data);
}

void PrimitiveBatcher::draw_rect(float x, float y, float w, float h, float angle, const RGB &color) {
  draw_rect(x, y, w, h, x + (w / 2), y + (h / 2), angle, color);
}

void PrimitiveBatcher::draw_rect(float x, float y, float w, float h, const RGB &color) {
  draw_rect(x, y, w, h, 0, 0, 0, color);
}

void PrimitiveBatcher::fill_square(float x, float y, float side_l, float rx, float ry, float angle, const RGB &color) {
  fill_rect(x, y, side_l, side_l, rx, ry, angle, color);
}

void PrimitiveBatcher::fill_square(float x, float y, float side_l, float angle, const RGB &color) {
  fill_rect(x, y, side_l, side_l, x + (side_l / 2), y + (side_l / 2), angle, color);
}

void PrimitiveBatcher::fill_square(float x, float y, float side_l, const RGB &color) {
  fill_rect(x, y, side_l, side_l, 0, 0, 0, color);
}

void PrimitiveBatcher::draw_square(float x, float y, float side_l, float rx, float ry, float angle, const RGB &color) {
  draw_rect(x, y, side_l, side_l, rx, ry, angle, color);
}

void PrimitiveBatcher::draw_square(float x, float y, float side_l, float angle, const RGB &color) {
  draw_rect(x, y, side_l, side_l, x + (side_l / 2), y + (side_l / 2), angle, color);
}

void PrimitiveBatcher::draw_square(float x, float y, float side_l, const RGB &color) {
  draw_rect(x, y, side_l, side_l, 0, 0, 0, color);
}

void PrimitiveBatcher::fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, float rx, float ry, float angle, const RGB &color) {
  auto cv = color.vec4();
  auto data = std::initializer_list<float>{
      x0, y0, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x1, y1, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x2, y2, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f) batcher->t_add_tri(data);
  else             batcher->o_add_tri(data);
}

void PrimitiveBatcher::fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, float angle, const RGB &color) {
  fill_tri(x0, y0, x1, y1, x2, y2, (x0 + x1 + x2) / 3, (y0 + y1 + y2) / 3, angle, color);
}

void PrimitiveBatcher::fill_tri(float x0, float y0, float x1, float y1, float x2, float y2, const RGB &color) {
  fill_tri(x0, y0, x1, y1, x2, y2, 0, 0, 0, color);
}

void PrimitiveBatcher::draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, float rx, float ry, float angle, const RGB &color) {
  auto cv = color.vec4();
  auto data = std::initializer_list<float>{
      x0, y0, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x1, y1, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),

      x1, y1, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x2, y2, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),

      x2, y2, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x0, y0, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f) batcher->t_add_line(data);
  else             batcher->o_add_line(data);
}

void PrimitiveBatcher::draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, float angle, const RGB &color) {
  draw_tri(x0, y0, x1, y1, x2, y2, (x0 + x1 + x2) / 3, (y0 + y1 + y2) / 3, angle, color);
}

void PrimitiveBatcher::draw_tri(float x0, float y0, float x1, float y1, float x2, float y2, const RGB &color) {
  draw_tri(x0, y0, x1, y1, x2, y2, 0, 0, 0, color);
}

void PrimitiveBatcher::fill_tri_equilateral(float cx, float cy, float radius, float rx, float ry, float angle, const RGB &color) {
  float x0 = cx + radius * std::cos(0);
  float y0 = cy + radius * std::sin(0);
  float x1 = cx + radius * std::cos(2.0944);
  float y1 = cy + radius * std::sin(2.0944);
  float x2 = cx + radius * std::cos(4.18879);
  float y2 = cy + radius * std::sin(4.18879);
  fill_tri(x0, y0, x1, y1, x2, y2, rx, ry, angle, color);
}

void PrimitiveBatcher::fill_tri_equilateral(float cx, float cy, float radius, float angle, const RGB &color) {
  fill_tri_equilateral(cx, cy, radius, cx, cy, angle, color);
}

void PrimitiveBatcher::fill_tri_equilateral(float cx, float cy, float radius, const RGB &color) {
  fill_tri_equilateral(cx, cy, radius, 0, 0, 0, color);
}

void PrimitiveBatcher::draw_tri_equilateral(float cx, float cy, float radius, float rx, float ry, float angle, const RGB &color) {
  float x0 = cx + radius * std::cos(0);
  float y0 = cy + radius * std::sin(0);
  float x1 = cx + radius * std::cos(2.0944);
  float y1 = cy + radius * std::sin(2.0944);
  float x2 = cx + radius * std::cos(4.18879);
  float y2 = cy + radius * std::sin(4.18879);
  draw_tri(x0, y0, x1, y1, x2, y2, rx, ry, angle, color);
}

void PrimitiveBatcher::draw_tri_equilateral(float cx, float cy, float radius, float angle, const RGB &color) {
  draw_tri_equilateral(cx, cy, radius, cx, cy, angle, color);
}

void PrimitiveBatcher::draw_tri_equilateral(float cx, float cy, float radius, const RGB &color) {
  draw_tri_equilateral(cx, cy, radius, 0, 0, 0, color);
}

void PrimitiveBatcher::line(float x0, float y0, float x1, float y1, float rx, float ry, float angle, const RGB &color) {
  auto cv = color.vec4();
  auto data = std::initializer_list<float>{
      x0, y0, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x1, y1, batcher->z,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f) batcher->t_add_line(data);
  else             batcher->o_add_line(data);
}

void PrimitiveBatcher::line(float x0, float y0, float x1, float y1, float angle, const RGB &color) {
  line(x0, y0, x1, y1, (x0 + x1) / 2, (y0 + y1) / 2, angle, color);
}

void PrimitiveBatcher::line(float x0, float y0, float x1, float y1, const RGB &color) {
  line(x0, y0, x1, y1, 0, 0, 0, color);
}

void PrimitiveBatcher::point(float x, float y, const RGB &color) {
  auto cv = color.vec4();
  auto data = std::initializer_list<float>{x, y, batcher->z,  cv.r, cv.g, cv.b, cv.a};
  if (cv.a < 1.0f) batcher->t_add_point(data);
  else             batcher->o_add_point(data);
}

void PrimitiveBatcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  batcher = module_mgr->get<Batcher>();

  Module::e_initialize_(e);
}

void PrimitiveBatcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

} // namespace baphy
