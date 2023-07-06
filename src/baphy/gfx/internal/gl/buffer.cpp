#include "baphy/gfx/internal/gl/buffer.hpp"

namespace baphy {

Buffer::Buffer(GfxContext &gfx) : gl(gfx.gl) {
  gen_id_();
}

Buffer::~Buffer() {
  del_id_();
}

Buffer::Buffer(Buffer &&other) noexcept : gl(other.gl), id(other.id) {
  other.id = 0;
}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  if (this != &other) {
    del_id_();

    gl = other.gl;
    id = other.id;

    other.id = 0;
  }
  return *this;
}

void Buffer::bind(const BufTarget &target) const {
  gl.BindBuffer(unwrap(target), id);
}

void Buffer::unbind(const BufTarget &target) const {
  gl.BindBuffer(unwrap(target), 0);
}

void Buffer::gen_id_() {
  gl.GenBuffers(1, &id);
  BAPHY_LOG_DEBUG("GEN_ID({}): Buffer", id);
}

void Buffer::del_id_() {
  if (id != 0) {
    gl.DeleteBuffers(1, &id);
    BAPHY_LOG_DEBUG("DEL_ID({}): Buffer", id);
    id = 0;
  }
}

} // namespace baphy
