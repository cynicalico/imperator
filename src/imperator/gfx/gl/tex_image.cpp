#include "imperator/gfx/gl/tex_image.hpp"
#include "imperator/util/io.hpp"

namespace imp {
TexImage::TexImage(GfxContext& gfx, const std::filesystem::path& path, bool retro) : gl(gfx.gl) {
  stbi_set_flip_vertically_on_load(false);

  gen_id_();
  bind();

  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);

  auto image_data = ImageData(path);
  if (image_data.bytes()) {
    GLenum format = GL_NONE;
    if (image_data.comp() == 3)
      format = GL_RGB;
    else if (image_data.comp() == 4)
      format = GL_RGBA;
    else
      IMP_LOG_ERROR("Can't handle images with comp '{}', only 3 or 4 channels supported", image_data.comp());

    if (format != GL_NONE) {
      w = image_data.w();
      h = image_data.h();

      gl.TexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, &image_data[0]);
      gl.GenerateMipmap(GL_TEXTURE_2D);

      IMP_LOG_DEBUG("Loaded texture '{}' ({}x{})", path.string(), w, h);
    }

    if (image_data.comp() == 4) {
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          int alpha_ch = (y * w * 4) + (x * 4) + 3;
          if (image_data[alpha_ch] < 255)
            fully_opaque = false;
        }
        if (!fully_opaque)
          break;
      }
    }
  }

  unbind();
}

TexImage::TexImage(GfxContext& gfx, TexFormat format, GLsizei w, GLsizei h, bool retro) : gl(gfx.gl), w(w), h(h) {
  gen_id_();
  bind();

  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  gl.TexStorage2D(GL_TEXTURE_2D, 1, unwrap(format), w, h);
  gl.GenerateMipmap(GL_TEXTURE_2D);
  fully_opaque = false;
  flipped = true;

  IMP_LOG_DEBUG("Created texture ({}x{})", w, h);

  unbind();
}

TexImage::~TexImage() {
  del_id_();
}

TexImage::TexImage(TexImage&& other) noexcept
  : gl(other.gl), id(other.id), w(other.w), h(other.h), fully_opaque(other.fully_opaque) {
  other.id = 0;
  other.w = 0;
  other.h = 0;
  other.fully_opaque = false;
}

TexImage& TexImage::operator=(TexImage&& other) noexcept {
  if (this != &other) {
    del_id_();

    gl = other.gl;
    id = other.id;
    w = other.w;
    h = other.h;
    fully_opaque = other.fully_opaque;

    other.id = 0;
    other.w = 0;
    other.h = 0;
    other.fully_opaque = false;
  }
  return *this;
}

void TexImage::bind(int unit) {
  gl.ActiveTexture(GL_TEXTURE0 + unit);
  gl.BindTexture(GL_TEXTURE_2D, id);
}

void TexImage::unbind() {
  gl.BindTexture(GL_TEXTURE_2D, 0);
}

void TexImage::gen_id_() {
  gl.GenTextures(1, &id);
  IMP_LOG_DEBUG("GEN_ID({}): TexImage", id);
}

void TexImage::del_id_() {
  if (id != 0) {
    gl.DeleteTextures(1, &id);
    IMP_LOG_DEBUG("DEL_ID({}): TexImage", id);
    id = 0;
  }
}
} // namespace imp
