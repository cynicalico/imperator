#include "imperator/gl/renderbuffer.h"

namespace imp {
Renderbuffer::Renderbuffer(GfxContext &gfx, RBufFormat format, GLuint width, GLuint height)
    : gl(gfx.gl),
      width(width),
      height(height) {
    gen_id_();

    bind();
    gl.RenderbufferStorage(GL_RENDERBUFFER, unwrap(format), width, height);
    unbind();
}

Renderbuffer::~Renderbuffer() {
    del_id_();
}

Renderbuffer::Renderbuffer(Renderbuffer &&other) noexcept
    : gl(other.gl),
      id(other.id),
      width(other.width),
      height(other.height) {
    other.id = 0;
    other.width = 0;
    other.height = 0;
}

Renderbuffer &Renderbuffer::operator=(Renderbuffer &&other) noexcept {
    if (this != &other) {
        del_id_();

        gl = other.gl;
        id = other.id;
        width = other.width;
        height = other.height;

        other.id = 0;
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

void Renderbuffer::bind() {
    gl.BindRenderbuffer(GL_RENDERBUFFER, id);
}

void Renderbuffer::unbind() {
    gl.BindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderbuffer::gen_id_() {
    gl.GenRenderbuffers(1, &id);
    IMPERATOR_LOG_DEBUG("GEN_ID({}): Renderbuffer", id);
}

void Renderbuffer::del_id_() {
    if (id != 0) {
        gl.DeleteRenderbuffers(1, &id);
        IMPERATOR_LOG_DEBUG("DEL_ID({}): Renderbuffer", id);
        id = 0;
    }
}
} // namespace imp
