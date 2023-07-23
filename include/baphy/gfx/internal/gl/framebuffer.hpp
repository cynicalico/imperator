#ifndef BAPHY_GFX_INTERNAL_GL_FRAMEBUFFER_HPP
#define BAPHY_GFX_INTERNAL_GL_FRAMEBUFFER_HPP

#include "baphy/gfx/internal/gl/renderbuffer.hpp"
#include "baphy/gfx/internal/gl/texture_unit.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace baphy {

enum class FboTarget {
  draw = GL_DRAW_FRAMEBUFFER,
  read = GL_READ_FRAMEBUFFER
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

  std::shared_ptr<Texture> get_texture(const std::string &tex_name);

private:
  std::unordered_map<std::string, std::shared_ptr<Texture>> tex_attachments_{};
  std::unordered_map<std::string, std::unique_ptr<Renderbuffer>> rbo_attachments_{};

  void del_id_();
};

class FramebufferBuilder {
public:
  GfxContext &gfx;
  TextureBatcher &textures;
  GladGLContext &gl;

  FramebufferBuilder(GfxContext &gfx, TextureBatcher &textures, GLsizei width, GLsizei height);

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

  std::unordered_map<std::string, std::shared_ptr<Texture>> tex_attachments_{};
  std::unordered_map<std::string, std::unique_ptr<Renderbuffer>> rbo_attachments_{};

  void gen_id_();

  GLenum get_texture_format(GLenum internalformat);
  GLenum get_renderbuffer_attachment_type(GLenum internalformat);
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_FRAMEBUFFER_HPP
