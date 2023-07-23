#include <utility>

#include "baphy/gfx/module/texture_batcher.hpp"

namespace baphy {

Texture::Texture(std::shared_ptr<TextureBatcher> mgr, std::unique_ptr<TextureUnit> &gl_obj)
    : mgr(std::move(mgr)), gl_obj_(std::move(gl_obj)) {
  id = gl_obj_->id;
  width = gl_obj_->width;
  height = gl_obj_->height;
  fully_opaque = gl_obj_->fully_opaque;
}

Texture::Texture(Texture &&other) noexcept : mgr(other.mgr), gl_obj_(std::move(other.gl_obj_)) {
  id = other.id;
  width = other.width;
  height = other.height;
  fully_opaque = other.fully_opaque;

  other.id = 0;
  other.width = 0;
  other.height = 0;
  other.fully_opaque = true;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if (this != &other) {
    mgr = other.mgr;
    gl_obj_ = std::move(other.gl_obj_);

    id = other.id;
    width = other.width;
    height = other.height;
    fully_opaque = other.fully_opaque;

    other.id = 0;
    other.width = 0;
    other.height = 0;
    other.fully_opaque = true;
  }
  return *this;
}

void Texture::draw(float x, float y) {
  mgr->draw_texture_(id, fully_opaque, gl_obj_->flipped, x, y, width, height, 0, 0, 1, 1, 0, 0, 0);
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
    float rx, float ry, float angle,
    const baphy::RGB &color
) {
  auto cv = color.vec4();
  std::initializer_list<float> data{
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  tx,      flipped ? ty + th : ty,  rx, ry, -glm::radians(angle),
      x + w, y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  tx + tw, flipped ? ty + th : ty,  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  tx + tw, flipped ? ty : ty + th,  rx, ry, -glm::radians(angle),
      x,     y,     batcher->z,  cv.r, cv.g, cv.b, cv.a,  tx,      flipped ? ty + th : ty,  rx, ry, -glm::radians(angle),
      x + w, y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  tx + tw, flipped ? ty : ty + th,  rx, ry, -glm::radians(angle),
      x,     y + h, batcher->z,  cv.r, cv.g, cv.b, cv.a,  tx,      flipped ? ty : ty + th,  rx, ry, -glm::radians(angle),
  };
  if (cv.a < 1.0f || !fully_opaque) batcher->t_add_tex(id, data);
  else                              batcher->o_add_tex(id, data);
}

} // namespace baphy
