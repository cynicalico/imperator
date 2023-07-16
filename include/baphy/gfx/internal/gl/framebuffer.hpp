#ifndef BAPHY_GFX_INTERNAL_GL_FRAMEBUFFER_HPP
#define BAPHY_GFX_INTERNAL_GL_FRAMEBUFFER_HPP

#include "baphy/gfx/module/gfx_context.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace baphy {

enum class FboTarget {
  draw = GL_DRAW_FRAMEBUFFER,
  read = GL_READ_FRAMEBUFFER
};

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

class Framebuffer {
  friend class FramebufferBuilder;

public:
  GladGLContext &gl;

  GLuint id{0};
  GLsizei width{0}, height{0};

  Framebuffer(GladGLContext &gl) : gl(gl) {}
  ~Framebuffer();

  // Copy constructors don't make sense for OpenGL objects
  Framebuffer(const Framebuffer &) = delete;
  Framebuffer &operator=(const Framebuffer &) = delete;

  Framebuffer(Framebuffer &&other) noexcept;
  Framebuffer &operator=(Framebuffer &&other) noexcept;

  void bind();
  void bind(FboTarget target);

  void unbind();

  void copy_to_default_framebuffer(bool retro = false);
  void copy_to_default_framebuffer(GLint window_width, GLint window_height, bool retro = false);

  void use_texture(const std::string &tex_name);

private:
  std::unordered_map<std::string, GLuint> tex_attachments_{};
  std::unordered_map<std::string, GLuint> rbo_attachments_{};

  void del_id_();
};

class FramebufferBuilder {
public:
  GladGLContext &gl;

  FramebufferBuilder(GfxContext &gfx, GLsizei width, GLsizei height);

  ~FramebufferBuilder() = default;

  FramebufferBuilder(const FramebufferBuilder &) = delete;
  FramebufferBuilder &operator=(const FramebufferBuilder &) = delete;

  FramebufferBuilder(FramebufferBuilder &&) noexcept = delete;
  FramebufferBuilder &operator=(FramebufferBuilder &&) noexcept = delete;

  FramebufferBuilder &texture(const std::string &tag, TexFormat internalformat, bool retro = false);
  FramebufferBuilder &texture(TexFormat internalformat);

  FramebufferBuilder &renderbuffer(const std::string &tag, RBufFormat internalformat);
  FramebufferBuilder &renderbuffer(RBufFormat internalformat);

  std::unique_ptr<Framebuffer> check_complete();

private:
  GLuint id_{0};
  GLsizei width_{0}, height_{0};

  std::unordered_map<std::string, GLuint> tex_attachments_{};
  std::unordered_map<std::string, GLuint> rbo_attachments_{};

  void gen_id_();

  GLenum get_texture_format(GLenum internalformat);

  GLenum get_renderbuffer_attachment_type(GLenum internalformat);
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_FRAMEBUFFER_HPP
