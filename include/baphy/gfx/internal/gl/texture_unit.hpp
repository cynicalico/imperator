#ifndef BAPHY_GFX_INTERNAL_GL_TEXTURE_UNIT_HPP
#define BAPHY_GFX_INTERNAL_GL_TEXTURE_UNIT_HPP

#include "baphy/gfx/module/gfx_context.hpp"
#include "glad/gl.h"
#include <filesystem>

namespace baphy {

enum class TexFormat {
  r = GL_RED,
  rg = GL_RG,
  rgb = GL_RGB,
  rgba = GL_RGBA,
  r8 = GL_R8,
  r8_snorm = GL_R8_SNORM,
  r16 = GL_R16,
  r16_snorm = GL_R16_SNORM,
  rg8 = GL_RG8,
  rg8_snorm = GL_RG8_SNORM,
  rg16 = GL_RG16,
  rg16_snorm = GL_RG16_SNORM,
  r3_g3_b2 = GL_R3_G3_B2,
  rgb4 = GL_RGB4,
  rgb5 = GL_RGB5,
  rgb8 = GL_RGB8,
  rgb8_snorm = GL_RGB8_SNORM,
  rgb10 = GL_RGB10,
  rgb12 = GL_RGB12,
  rgb16_snorm = GL_RGB16_SNORM,
  rgba2 = GL_RGBA2,
  rgba4 = GL_RGBA4,
  rgb5_a1 = GL_RGB5_A1,
  rgba8 = GL_RGBA8,
  rgba8_snorm = GL_RGBA8_SNORM,
  rgb10_a2 = GL_RGB10_A2,
  rgb10_a2ui = GL_RGB10_A2UI,
  rgba12 = GL_RGBA12,
  rgba16 = GL_RGBA16,
  srgb8 = GL_SRGB8,
  srgb8_alpha8 = GL_SRGB8_ALPHA8,
  r16f = GL_R16F,
  rg16f = GL_RG16F,
  rgb16f = GL_RGB16F,
  rgba16f = GL_RGBA16F,
  r32f = GL_R32F,
  rg32f = GL_RG32F,
  rgb32f = GL_RGB32F,
  rgba32f = GL_RGBA32F,
  r11f_g11f_b10f = GL_R11F_G11F_B10F,
  rgb9_e5 = GL_RGB9_E5,
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
  rgb8i = GL_RGB8I,
  rgb8ui = GL_RGB8UI,
  rgb16i = GL_RGB16I,
  rgb16ui = GL_RGB16UI,
  rgb32i = GL_RGB32I,
  rgb32ui = GL_RGB32UI,
  rgba8i = GL_RGBA8I,
  rgba8ui = GL_RGBA8UI,
  rgba16i = GL_RGBA16I,
  rgba16ui = GL_RGBA16UI,
  rgba32i = GL_RGBA32I,
  rgba32ui = GL_RGBA32UI,
  compressed_red_rgtc1 = GL_COMPRESSED_RED_RGTC1,
  compressed_signed_red_rgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
  compressed_rg_rgtc2 = GL_COMPRESSED_RG_RGTC2,
  compressed_signed_rg_rgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
  compressed_rgba_bptc_unorm = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,
  compressed_srgb_alpha_bptc_unorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,
  compressed_rgb_bptc_signed_float = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,
  compressed_rgb_bptc_unsigned_float = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,
  compressed_rgb8_etc2 = GL_COMPRESSED_RGB8_ETC2,
  compressed_srgb8_etc2 = GL_COMPRESSED_SRGB8_ETC2,
  compressed_rgb8_punchthrough_alpha1_etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
  compressed_srgb8_punchthrough_alpha1_etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
  compressed_rgba8_etc2_eac = GL_COMPRESSED_RGBA8_ETC2_EAC,
  compressed_srgb8_alpha8_etc2_eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
  compressed_r11_eac = GL_COMPRESSED_R11_EAC,
  compressed_signed_r11_eac = GL_COMPRESSED_SIGNED_R11_EAC,
  compressed_rg11_eac = GL_COMPRESSED_RG11_EAC,
  compressed_signed_rg11_eac = GL_COMPRESSED_SIGNED_RG11_EAC,
  d16 = GL_DEPTH_COMPONENT16,
  d24 = GL_DEPTH_COMPONENT24,
  d32f = GL_DEPTH_COMPONENT32F,
  d24_s8 = GL_DEPTH24_STENCIL8,
  d32f_s8 = GL_DEPTH32F_STENCIL8,
  s8 = GL_STENCIL_INDEX8
};

class TextureUnit {
public:
  GladGLContext &gl;

  GLuint id{0};
  GLuint width{0};
  GLuint height{0};
  bool fully_opaque{true};

  TextureUnit(GfxContext &gfx, const std::filesystem::path &path, bool retro = false);
  TextureUnit(GfxContext &gfx, TexFormat format, GLuint width, GLuint height, bool retro = false);
  ~TextureUnit();

  TextureUnit(const TextureUnit &) = delete;
  TextureUnit &operator=(const TextureUnit &) = delete;

  TextureUnit(TextureUnit &&other) noexcept;
  TextureUnit &operator=(TextureUnit &&other) noexcept;

  void bind(int unit = 0);
  void unbind();

private:
  void gen_id_();
  void del_id_();
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_TEXTURE_UNIT_HPP
