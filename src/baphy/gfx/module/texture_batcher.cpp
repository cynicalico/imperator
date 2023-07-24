#include <utility>

#include "baphy/gfx/module/texture_batcher.hpp"

namespace baphy {

Texture::Texture(std::shared_ptr<TextureBatcher> mgr, std::unique_ptr<TextureUnit> &gl_obj)
    : mgr(std::move(mgr)), gl_obj_(std::move(gl_obj)) {
  id = gl_obj_->id;
  px_w = 1.0f / gl_obj_->w;
  px_h = 1.0f / gl_obj_->h;
}

Texture::Texture(Texture &&other) noexcept : mgr(std::move(other.mgr)), gl_obj_(std::move(other.gl_obj_)) {
  id = other.id;
  px_w = other.px_w;
  px_h = other.px_h;

  other.id = 0;
  other.px_w = 0;
  other.px_h = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if (this != &other) {
    mgr = std::move(other.mgr);
    gl_obj_ = std::move(other.gl_obj_);

    id = other.id;
    px_w = other.px_w;
    px_h = other.px_h;

    other.id = 0;
    other.px_w = 0;
    other.px_h = 0;
  }
  return *this;
}

GLsizei Texture::w() const {
  return gl_obj_->w;
}

GLsizei Texture::h() const {
  return gl_obj_->h;
}

void Texture::draw(float x, float y, float w, float h, float tx, float ty, float tw, float th, float rx, float ry, float angle, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, w, h, tx, ty, tw, th, px_w, px_h, rx, ry, angle, color);
}

void Texture::draw(float x, float y, float w, float h, float tx, float ty, float tw, float th, float angle, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, w, h, tx, ty, tw, th, px_w, px_h,  x + (w / 2.0), y + (h / 2.0), angle, color);
}

void Texture::draw(float x, float y, float w, float h, float tx, float ty, float tw, float th, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, w, h, tx, ty, tw, th, px_w, px_h, 0, 0, 0, color);
}

void Texture::draw(float x, float y, float w, float h, float rx, float ry, float angle, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, w, h, 0, 0, gl_obj_->w, gl_obj_->h, px_w, px_h, rx, ry, angle, color);
}

void Texture::draw(float x, float y, float w, float h, float angle, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, w, h, 0, 0, gl_obj_->w, gl_obj_->h, px_w, px_h, x + (w / 2.0), y + (h / 2.0), angle, color);
}

void Texture::draw(float x, float y, float w, float h, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, w, h, 0, 0, gl_obj_->w, gl_obj_->h, px_w, px_h, 0, 0, 0, color);
}

void Texture::draw(float x, float y, float angle, const RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, gl_obj_->w, gl_obj_->h, 0, 0, gl_obj_->w, gl_obj_->h, px_w, px_h, x + (gl_obj_->w / 2.0), y + (gl_obj_->h / 2.0), angle, color);
}

void Texture::draw(float x, float y, const baphy::RGB &color) {
  mgr->draw_texture_(id, gl_obj_->fully_opaque, gl_obj_->flipped, x, y, gl_obj_->w, gl_obj_->h, 0, 0, gl_obj_->w, gl_obj_->h, px_w, px_h, 0, 0, 0, color);
}

std::shared_ptr<Texture> TextureBatcher::load(const std::filesystem::path &path, bool retro) {
  auto gl_obj = std::make_unique<TextureUnit>(*gfx, path, retro);
  textures_.emplace_back(std::make_shared<Texture>(shared_from_this(), gl_obj));

  return textures_.back();
}

std::shared_ptr<Texture> TextureBatcher::create(TexFormat format, GLuint width, GLuint height, bool retro) {
  auto gl_obj = std::make_unique<TextureUnit>(*gfx, format, width, height, retro);
  textures_.emplace_back(std::make_shared<Texture>(shared_from_this(), gl_obj));

  return textures_.back();
}

void TextureBatcher::e_initialize_(const EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  batcher = module_mgr->get<Batcher>();

  Module::e_initialize_(e);
}

void TextureBatcher::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void TextureBatcher::draw_texture_(
    GLuint id, bool fully_opaque, bool flipped,
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    float x_px_unit, float y_px_unit,
    float rx, float ry, float angle,
    const baphy::RGB &color
) {
  auto cv = color.vec4();
  std::initializer_list<float> data{
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  x_px_unit * tx,        y_px_unit * (flipped ? ty + th : ty),  rx, ry, -glm::radians(angle),
      x + w, y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  x_px_unit * (tx + tw), y_px_unit * (flipped ? ty + th : ty),  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  x_px_unit * (tx + tw), y_px_unit * (flipped ? ty : ty + th),  rx, ry, -glm::radians(angle),
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  x_px_unit * tx,        y_px_unit * (flipped ? ty + th : ty),  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  x_px_unit * (tx + tw), y_px_unit * (flipped ? ty : ty + th),  rx, ry, -glm::radians(angle),
      x,     y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  x_px_unit * tx,        y_px_unit * (flipped ? ty : ty + th),  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f || !fully_opaque) batcher->t_add_tex(id, data);
  else                              batcher->o_add_tex(id, data);
}

} // namespace baphy
