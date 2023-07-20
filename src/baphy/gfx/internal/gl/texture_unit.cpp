#include "baphy/gfx/internal/gl/texture_unit.hpp"

#include "stb_image.h"

namespace baphy {

TextureUnit::TextureUnit(GfxContext &gfx, const std::filesystem::path &path, bool retro) : gl(gfx.gl) {
  stbi_set_flip_vertically_on_load(false);

  gen_id_();
  bind();

  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);

  int w, h, comp;
  auto bytes = stbi_load(path.string().c_str(), &w, &h, &comp, 0);

  if (bytes) {
    width = w;
    height = h;

    GLenum format = GL_NONE;
    if (comp == 3)
      format = GL_RGB;
    else if (comp == 4)
      format = GL_RGBA;
    else
      BAPHY_LOG_ERROR("Can't handle images with comp '{}', only 3 or 4 channels supported", comp);

    if (format != GL_NONE) {
      gl.TexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
      gl.GenerateMipmap(GL_TEXTURE_2D);

      BAPHY_LOG_DEBUG("Loaded texture '{}' ({}x{})", path.string(), width, height);
    }

    if (comp == 4) {
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          int alpha_ch = (y * w * 4) + (x * 4) + 3;
          if (bytes[alpha_ch] < 255)
            fully_opaque = false;
        }
        if (!fully_opaque)
          break;
      }
    }

  } else
    BAPHY_LOG_ERROR("Failed to load texture unit '{}': {}", path.string(), stbi_failure_reason());

  stbi_image_free(bytes);
  unbind();
}

TextureUnit::TextureUnit(GfxContext &gfx, TexFormat format, GLuint width, GLuint height, bool retro) : gl(gfx.gl), width(width), height(height) {
  gen_id_();
  bind();

  gl.TexStorage2D(GL_TEXTURE_2D, 1, unwrap(format), width, height);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);

  BAPHY_LOG_DEBUG("Created texture ({}x{})", width, height);

  unbind();
}

TextureUnit::~TextureUnit() {
  del_id_();
}

TextureUnit::TextureUnit(TextureUnit &&other) noexcept
    : gl(other.gl), id(other.id), width(other.width), height(other.height), fully_opaque(other.fully_opaque) {
  other.id = 0;
  other.width = 0;
  other.height = 0;
  other.fully_opaque = false;
}

TextureUnit &TextureUnit::operator=(TextureUnit &&other) {
  if (this != &other) {
    del_id_();

    gl = other.gl;
    id = other.id;
    width = other.width;
    height = other.height;
    fully_opaque = other.fully_opaque;

    other.id = 0;
    other.width = 0;
    other.height = 0;
    other.fully_opaque = false;
  }
  return *this;
}

void TextureUnit::bind(int unit) {
  gl.ActiveTexture(GL_TEXTURE0 + unit);
  gl.BindTexture(GL_TEXTURE_2D, id);
}

void TextureUnit::unbind() {
  gl.BindTexture(GL_TEXTURE_2D, 0);
}

void TextureUnit::gen_id_() {
  gl.GenTextures(1, &id);
  BAPHY_LOG_DEBUG("GEN_ID({}): TextureUnit", id);
}

void TextureUnit::del_id_() {
  if (id != 0) {
    gl.DeleteTextures(1, &id);
    BAPHY_LOG_DEBUG("DEL_ID({}): TextureUnit", id);
    id = 0;
  }
}

} // namespace baphy