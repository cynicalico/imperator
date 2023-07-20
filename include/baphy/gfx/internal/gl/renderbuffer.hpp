#ifndef BAPHY_GFX_INTERNAL_GL_RENDERBUFFER_HPP
#define BAPHY_GFX_INTERNAL_GL_RENDERBUFFER_HPP

#include "baphy/gfx/module/gfx_context.hpp"

namespace baphy {

enum class RBufFormat {
  r8 = GL_R8,
  r16 = GL_R16,
  rg8 = GL_RG8,
  rg16 = GL_RG16,
  rgb565 = GL_RGB565,
  rgba4 = GL_RGBA4,
  rgb5_a1 = GL_RGB5_A1,
  rgba8 = GL_RGBA8,
  rgb10_a2 = GL_RGB10_A2,
  rgb10_a2ui = GL_RGB10_A2UI,
  rgba16 = GL_RGBA16,
  srgb8_a8 = GL_SRGB8_ALPHA8,
  r16f = GL_R16F,
  rg16f = GL_RG16F,
  rgba16f = GL_RGBA16F,
  r32f = GL_R32F,
  rg32f = GL_RG32F,
  rgba32f = GL_RGBA32F,
  r11f_g11f_b10f = GL_R11F_G11F_B10F,
  r8i = GL_R8I,
  r8ui = GL_R8UI,
  r16i = GL_R16I,
  r16ui = GL_R16UI,
  r32i = GL_R32I,
  r32ui = GL_R32UI,
  rg8i = GL_RG8I,
  rg8ui = GL_RG8UI,
  rg16i = GL_RG16I,
  rg16ui = GL_RG16UI,
  rg32i = GL_RG32I,
  rg32ui = GL_RG32UI,
  rgba8i = GL_RGBA8I,
  rgba8ui = GL_RGBA8UI,
  rgba16i = GL_RGBA16I,
  rgba16ui = GL_RGBA16UI,
  rgba32i = GL_RGBA32I,
  rgba32ui = GL_RGBA32UI,
  d16 = GL_DEPTH_COMPONENT16,
  d24 = GL_DEPTH_COMPONENT24,
  d32f = GL_DEPTH_COMPONENT32F,
  d24_s8 = GL_DEPTH24_STENCIL8,
  d32f_s8 = GL_DEPTH32F_STENCIL8,
  s8 = GL_STENCIL_INDEX8
};

class Renderbuffer {
public:
  GladGLContext &gl;

  GLuint id{0};
  GLuint width{0};
  GLuint height{0};

  Renderbuffer(GfxContext &gfx, RBufFormat format, GLuint width, GLuint height);
  ~Renderbuffer();

  Renderbuffer(const Renderbuffer &) = delete;
  Renderbuffer &operator=(const Renderbuffer &) = delete;

  Renderbuffer(Renderbuffer &&other) noexcept;
  Renderbuffer &operator=(Renderbuffer &&other);

  void bind();
  void unbind();

private:
  void gen_id_();
  void del_id_();
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_RENDERBUFFER_HPP
