#include "baphy/gfx/internal/gl/buffer.hpp"

namespace baphy {

Buffer::Buffer(std::shared_ptr<GfxContext> gfx) : gfx(std::move(gfx)) {
  gen_id_();
}

Buffer::~Buffer() {
  del_id_();
}

Buffer::Buffer(Buffer &&other) noexcept : gfx(std::move(other.gfx)), id(other.id) {
  other.id = 0;
}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  if (this != &other) {
    del_id_();

    gfx = other.gfx;
    id = other.id;

    other.gfx = nullptr;
    other.id = 0;
  }
  return *this;
}

void Buffer::bind(const BufTarget &target) const {
  gfx->gl->BindBuffer(unwrap(target), id);
}

void Buffer::unbind(const BufTarget &target) const {
  gfx->gl->BindBuffer(unwrap(target), 0);
}

void Buffer::gen_id_() {
  gfx->gl->GenBuffers(1, &id);
  BAPHY_LOG_DEBUG("GEN_ID({}): Buffer", id);
}

void Buffer::del_id_() {
  if (id != 0) {
    gfx->gl->DeleteBuffers(1, &id);
    BAPHY_LOG_DEBUG("DEL_ID({}): Buffer", id);
    id = 0;
  }
}

} // namespace baphy
