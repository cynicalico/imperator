#include "imperator/gl/framebuffer.h"
#include "imperator/util/enum_bitops.h"
#include "imperator/util/log.h"
#include "imperator/util/rnd.h"

namespace imp {
Framebuffer::~Framebuffer() { del_id_(); }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept : gl(other.gl) {
    id = other.id;
    width = other.width;
    height = other.height;
    tex_attachments_ = std::move(other.tex_attachments_);
    rbo_attachments_ = std::move(other.rbo_attachments_);

    other.id = 0;
    other.width = 0;
    other.height = 0;
    other.tex_attachments_.clear();
    other.rbo_attachments_.clear();
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
    if (this != &other) {
        del_id_();

        gl = other.gl;
        id = other.id;
        width = other.width;
        height = other.height;
        tex_attachments_ = std::move(other.tex_attachments_);
        rbo_attachments_ = std::move(other.rbo_attachments_);

        other.id = 0;
        other.width = 0;
        other.height = 0;
        other.tex_attachments_.clear();
        other.rbo_attachments_.clear();
    }
    return *this;
}

void Framebuffer::bind() {
    gl.BindFramebuffer(GL_FRAMEBUFFER, id);
    gl.Viewport(0, 0, width, height);
}

void Framebuffer::bind(FboTarget target) { gl.BindFramebuffer(unwrap(target), id); }

void Framebuffer::unbind() { gl.BindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::copy_to_default_framebuffer(bool retro) {
    bind(FboTarget::read);
    gl.BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    gl.BlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, retro ? GL_NEAREST : GL_LINEAR);
    gl.BindFramebuffer(unwrap(FboTarget::read), 0);
    gl.BindFramebuffer(unwrap(FboTarget::draw), 0);
}

void Framebuffer::copy_to_default_framebuffer(GLint window_width, GLint window_height, bool retro) {
    bind(FboTarget::read);
    gl.BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    gl.BlitFramebuffer(
            0,
            0,
            width,
            height,
            0,
            0,
            window_width,
            window_height,
            GL_COLOR_BUFFER_BIT,
            retro ? GL_NEAREST : GL_LINEAR
    );
    gl.BindFramebuffer(unwrap(FboTarget::read), 0);
    gl.BindFramebuffer(unwrap(FboTarget::draw), 0);
}

const TextureUnit *Framebuffer::get_texture(const std::string &tex_name) { return tex_attachments_[tex_name].get(); }

const Renderbuffer *Framebuffer::get_renderbuffer(const std::string &rbo_name) {
    return rbo_attachments_[rbo_name].get();
}

void Framebuffer::del_id_() {
    if (id != 0) {
        tex_attachments_.clear();
        rbo_attachments_.clear();
        gl.DeleteFramebuffers(1, &id);
        IMPERATOR_LOG_DEBUG("DEL_ID({}): Framebuffer", id);
    }
}

FramebufferBuilder::FramebufferBuilder(GfxContext &gfx, GLsizei width, GLsizei height)
    : gfx(gfx), gl(gfx.gl), width_(width), height_(height) {
    gen_id_();
    gl.BindFramebuffer(GL_FRAMEBUFFER, id_);
}

FramebufferBuilder &FramebufferBuilder::texture(const std::string &tag, TexFormat internalformat, bool retro) {
    tex_attachments_[tag] = std::make_unique<TextureUnit>(gfx, internalformat, width_, height_, retro);
    gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_attachments_[tag]->id, 0);

    return *this;
}

FramebufferBuilder &FramebufferBuilder::texture(TexFormat format, bool retro) {
    return texture(rnd::base58(11), format, retro);
}

FramebufferBuilder &FramebufferBuilder::renderbuffer(const std::string &tag, RBufFormat internalformat) {
    rbo_attachments_[tag] = std::make_unique<Renderbuffer>(gfx, internalformat, width_, height_);

    auto attachment_type = get_renderbuffer_attachment_type(unwrap(internalformat));
    gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_type, GL_RENDERBUFFER, rbo_attachments_[tag]->id);

    return *this;
}

FramebufferBuilder &FramebufferBuilder::renderbuffer(RBufFormat internalformat) {
    return renderbuffer(rnd::base58(11), internalformat);
}

std::unique_ptr<Framebuffer> FramebufferBuilder::check_complete() {
    if (gl.CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        IMPERATOR_LOG_ERROR("Framebuffer is not complete");
    } else {
        IMPERATOR_LOG_DEBUG("Completed framebuffer ({})", id_);
    }
    gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    auto f = std::make_unique<Framebuffer>(gl);
    f->id = id_;
    f->width = width_;
    f->height = height_;
    f->tex_attachments_ = std::move(tex_attachments_);
    f->rbo_attachments_ = std::move(rbo_attachments_);

    return f;
}

void FramebufferBuilder::gen_id_() {
    gl.GenFramebuffers(1, &id_);
    IMPERATOR_LOG_DEBUG("GEN_ID({}): Framebuffer ", id_);
}

GLenum FramebufferBuilder::get_texture_format(GLenum internalformat) {
    switch (internalformat) {
    case GL_R8:
    case GL_R8_SNORM:
    case GL_R16:
    case GL_R16_SNORM:
    case GL_R16F:
    case GL_R32F:
    case GL_R8I:
    case GL_R8UI:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
    case GL_COMPRESSED_RED_RGTC1:
    case GL_COMPRESSED_SIGNED_RED_RGTC1:
    case GL_COMPRESSED_R11_EAC:
    case GL_COMPRESSED_SIGNED_R11_EAC:   return GL_RED;

    case GL_RG8:
    case GL_RG8_SNORM:
    case GL_RG16:
    case GL_RG16_SNORM:
    case GL_RG16F:
    case GL_RG32F:
    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG32I:
    case GL_RG32UI:
    case GL_COMPRESSED_RG_RGTC2:
    case GL_COMPRESSED_SIGNED_RG_RGTC2:
    case GL_COMPRESSED_RG11_EAC:
    case GL_COMPRESSED_SIGNED_RG11_EAC: return GL_RG;

    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB565:
    case GL_RGB8:
    case GL_RGB8_SNORM:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGB16_SNORM:
    case GL_SRGB8:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_R11F_G11F_B10F:
    case GL_RGB9_E5:
    case GL_RGB8I:
    case GL_RGB8UI:
    case GL_RGB16I:
    case GL_RGB16UI:
    case GL_RGB32I:
    case GL_RGB32UI:
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:
    case GL_COMPRESSED_RGB8_ETC2:
    case GL_COMPRESSED_SRGB8_ETC2:
    case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2: return GL_RGB;

    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGBA8_SNORM:
    case GL_RGB10_A2:
    case GL_RGB10_A2UI:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGBA16_SNORM:
    case GL_SRGB8_ALPHA8:
    case GL_RGBA16F:
    case GL_RGBA32F:
    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA32I:
    case GL_RGBA32UI:
    case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_RGBA8_ETC2_EAC:
    case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:     return GL_RGBA;

    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F: return GL_DEPTH_COMPONENT;

    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8: return GL_DEPTH_STENCIL;

    case GL_STENCIL_INDEX8: return GL_STENCIL_INDEX;

    default: IMPERATOR_LOG_ERROR("Bad value given to get_texture_format: {}", internalformat); return GL_RGBA;
    }
}

GLenum FramebufferBuilder::get_renderbuffer_attachment_type(GLenum internalformat) {
    switch (internalformat) {
    case GL_STENCIL_INDEX8: return GL_STENCIL_ATTACHMENT;

    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F: return GL_DEPTH_ATTACHMENT;

    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;

    default: return GL_COLOR_ATTACHMENT0;
    }
}
} // namespace imp
