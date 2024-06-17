#ifndef IMPERATOR_GL_FRAMEBUFFER_H
#define IMPERATOR_GL_FRAMEBUFFER_H

#include "imperator/gl/renderbuffer.h"
#include "imperator/gl/gl_texture.h"
#include "imperator/module/gfx/gfx_context.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace imp {
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

    const TextureUnit *get_texture(const std::string &tex_name);

    const Renderbuffer *get_renderbuffer(const std::string &rbo_name);

private:
    std::unordered_map<std::string, std::unique_ptr<TextureUnit>> tex_attachments_{};
    std::unordered_map<std::string, std::unique_ptr<Renderbuffer>> rbo_attachments_{};

    void del_id_();
};

class FramebufferBuilder {
public:
    GfxContext &gfx;
    GladGLContext &gl;

    FramebufferBuilder(GfxContext &gfx, GLsizei width, GLsizei height);

    ~FramebufferBuilder() = default;

    FramebufferBuilder(const FramebufferBuilder &) = delete;

    FramebufferBuilder &operator=(const FramebufferBuilder &) = delete;

    FramebufferBuilder(FramebufferBuilder &&) noexcept = delete;

    FramebufferBuilder &operator=(FramebufferBuilder &&) noexcept = delete;

    FramebufferBuilder &texture(const std::string &tag, TexFormat internalformat, bool retro = false);

    FramebufferBuilder &texture(TexFormat internalformat, bool retro = false);

    FramebufferBuilder &renderbuffer(const std::string &tag, RBufFormat internalformat);

    FramebufferBuilder &renderbuffer(RBufFormat internalformat);

    std::unique_ptr<Framebuffer> check_complete();

private:
    GLuint id_{0};
    GLsizei width_{0}, height_{0};

    std::unordered_map<std::string, std::unique_ptr<TextureUnit>> tex_attachments_{};
    std::unordered_map<std::string, std::unique_ptr<Renderbuffer>> rbo_attachments_{};

    void gen_id_();

    GLenum get_texture_format(GLenum internalformat);

    GLenum get_renderbuffer_attachment_type(GLenum internalformat);
};
} // namespace imp

#endif//IMPERATOR_GL_FRAMEBUFFER_H
