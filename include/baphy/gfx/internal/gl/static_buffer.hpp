#ifndef BAPHY_GFX_INTERNAL_GL_STATIC_BUFFER_HPP
#define BAPHY_GFX_INTERNAL_GL_STATIC_BUFFER_HPP

#include "baphy/gfx/internal/gl/buffer.hpp"
#include <concepts>
#include <vector>

namespace baphy {

template<typename T>
concept Numeric = requires(T) { std::integral<T> || std::floating_point<T>; };

template<Numeric T = float>
class StaticBuffer : public Buffer {
public:
  StaticBuffer(
      std::shared_ptr<GfxContext> gfx,
      const BufTarget &target,
      const BufUsage &usage,
      const std::vector<T> &data
  );
  ~StaticBuffer() override = default;

  // Copy constructors don't make sense for OpenGL objects
  StaticBuffer(const StaticBuffer &) = delete;
  StaticBuffer &operator=(const StaticBuffer &) = delete;

  StaticBuffer(StaticBuffer &&other) noexcept;
  StaticBuffer &operator=(StaticBuffer &&other) noexcept;

  void write(
      const BufTarget &target,
      const BufUsage &usage,
      const std::vector<T> &data
  );

  void write(const std::vector<T> &data);

  void write_sub(
      const BufTarget &target,
      const BufUsage &usage,
      std::size_t offset,
      const std::vector<T> &data
  );

  void write_sub(std::size_t offset, const std::vector<T> &data);

private:
  BufTarget last_target{BufTarget::none};
  BufUsage last_usage{BufUsage::none};
};

template<Numeric T>
StaticBuffer<T>::StaticBuffer(
    std::shared_ptr<GfxContext> gfx,
    const BufTarget &target,
    const BufUsage &usage,
    const std::vector<T> &data
) : Buffer(gfx) {
  write(target, usage, data);
}

template<Numeric T>
StaticBuffer<T>::StaticBuffer(StaticBuffer &&other) noexcept : Buffer(std::move(other)) {
  std::swap(last_target, other.last_target);
  std::swap(last_usage, other.last_usage);
}

template<Numeric T>
StaticBuffer<T> &StaticBuffer<T>::operator=(StaticBuffer &&other) noexcept {
  if (this != &other) {
    Buffer::operator=(std::move(other));

    std::swap(last_target, other.last_target);
    std::swap(last_usage, other.last_usage);
  }
  return *this;
}

template<Numeric T>
void StaticBuffer<T>::write(
    const BufTarget &target,
    const BufUsage &usage,
    const std::vector<T> &data
) {
  bind(target);
  gfx->gl->BufferData(unwrap(target), sizeof(T) * data.size(), &data[0], unwrap(usage));
  unbind(target);

  last_target = target;
  last_usage = usage;
}

template<Numeric T>
void StaticBuffer<T>::write(const std::vector<T> &data) {
  write(last_target, last_usage, 0, data);
}

template<Numeric T>
void StaticBuffer<T>::write_sub(
    const BufTarget &target,
    const BufUsage &usage,
    std::size_t offset,
    const std::vector<T> &data
) {
  bind(target);
  gfx->gl->BufferSubData(unwrap(target), sizeof(T) * offset, sizeof(T) * data.size(), &data[0]);
  unbind(target);

  last_target = target;
  last_usage = usage;
}

template<Numeric T>
void StaticBuffer<T>::write_sub(std::size_t offset, const std::vector<T> &data) {
  write_sub(last_target, last_usage, offset, data);
}

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_STATIC_BUFFER_HPP
