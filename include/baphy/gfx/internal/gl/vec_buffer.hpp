#ifndef BAPHY_GFX_INTERNAL_GL_VEC_BUFFER_HPP
#define BAPHY_GFX_INTERNAL_GL_VEC_BUFFER_HPP

#include "baphy/gfx/internal/gl/buffer.hpp"
#include <concepts>
#include <vector>

namespace baphy {

template<Numeric T = float>
class VecBuffer : public Buffer {
public:
  VecBuffer(
      GfxContext &gfx,
      std::size_t initial_size, bool fill_reverse,
      BufTarget target, BufUsage usage
  );
  ~VecBuffer() override = default;

  // Copy constructors don't make sense for OpenGL objects
  VecBuffer(const VecBuffer &) = delete;
  VecBuffer &operator=(const VecBuffer &) = delete;

  VecBuffer(VecBuffer &&other) noexcept;
  VecBuffer &operator=(VecBuffer &&other) noexcept;

  void clear();

  std::size_t front() const;

  std::size_t size() const;

  void add(const std::vector<T> &new_data);
  void add(std::initializer_list<T> new_data);

  void sync();

private:
  BufTarget target_{BufTarget::none};
  BufUsage usage_{BufUsage::none};

  std::vector<T> data_{};
  std::size_t front_{0}, back_{0};
  bool fill_reverse_{false};

  GLuint gl_bufsize_{0u}, gl_bufpos_{0u};

  template<typename InputIt>
  void add_(InputIt begin, InputIt end);
};

using FVBuffer = VecBuffer<float>;
using IVBuffer = VecBuffer<int>;
using UVBuffer = VecBuffer<unsigned int>;

template<Numeric T>
VecBuffer<T>::VecBuffer(
    GfxContext &gfx,
    std::size_t initial_size, bool fill_reverse,
    BufTarget target, BufUsage usage
) : Buffer(gfx), target_(target), usage_(usage), fill_reverse_(fill_reverse) {
  data_.resize(initial_size);
  if (fill_reverse_) {
    front_ = initial_size;
    back_ = initial_size;
  }
}

template<Numeric T>
VecBuffer<T>::VecBuffer(VecBuffer<T> &&other) noexcept : Buffer(std::move(other)) {
  target_ = other.target_;
  usage_ = other.usage_;
  data_ = other.data_;
  front_ = other.front_;
  back_ = other.back_;
  fill_reverse_ = other.fill_reverse_;
  gl_bufsize_ = other.gl_bufsize_;
  gl_bufpos_ = other.gl_bufpos_;

  other.target_ = BufTarget::none;
  other.usage_ = BufUsage::none;
  other.data_.clear();
  other.front_ = 0;
  other.back_ = 0;
  other.fill_reverse_ = false;
  other.gl_bufsize_ = 0;
  other.gl_bufpos_ = 0;
}

template<Numeric T>
VecBuffer<T> &VecBuffer<T>::operator=(VecBuffer<T> &&other) noexcept {
  if (this != &other) {
    Buffer::operator=(std::move(other));

    target_ = other.target_;
    usage_ = other.usage_;
    data_ = other.data_;
    front_ = other.front_;
    back_ = other.back_;
    fill_reverse_ = other.fill_reverse_;
    gl_bufsize_ = other.gl_bufsize_;
    gl_bufpos_ = other.gl_bufpos_;

    other.target_ = BufTarget::none;
    other.usage_ = BufUsage::none;
    other.data_.clear();
    other.front_ = 0;
    other.back_ = 0;
    other.fill_reverse_ = false;
    other.gl_bufsize_ = 0;
    other.gl_bufpos_ = 0;
  }
  return *this;
}

template<Numeric T>
void VecBuffer<T>::clear() {
  if (fill_reverse_) {
    front_ = data_.size();
    back_ = data_.size();
    gl_bufpos_ = front_;

  } else {
    front_ = 0u;
    back_ = 0u;
    gl_bufpos_ = back_;
  }
}

template<Numeric T>
std::size_t VecBuffer<T>::front() const {
  return front_;
}

template<Numeric T>
std::size_t VecBuffer<T>::size() const {
  return back_ - front_;
}

template<Numeric T>
void VecBuffer<T>::add(const std::vector<T> &new_data) {
  add_(new_data.begin(), new_data.end());
}

template<Numeric T>
void VecBuffer<T>::add(std::initializer_list<T> new_data) {
  add_(new_data.begin(), new_data.end());
}

template<Numeric T>
void VecBuffer<T>::sync() {
  if (gl_bufsize_ < data_.size()) {
    bind(target_);
    gl.BufferData(
        unwrap(target_),
        sizeof(T) * data_.size(),
        &data_[0],
        unwrap(usage_)
    );
    unbind(target_);

    gl_bufsize_ = data_.size();
    gl_bufpos_ = fill_reverse_ ? front_ : back_;

  } else {
    if (fill_reverse_ && gl_bufpos_ > front_) {
      bind(target_);
      gl.BufferSubData(
          unwrap(target_),
          sizeof(T) * front_,
          sizeof(T) * (gl_bufpos_ - front_),
          &data_[0] + front_
      );
      unbind(target_);

      gl_bufpos_ = front_;

    } else if (!fill_reverse_ && gl_bufpos_ < back_) {
      bind(target_);
      gl.BufferSubData(
          unwrap(target_),
          sizeof(T) * gl_bufpos_,
          sizeof(T) * (back_ - gl_bufpos_),
          &data_[0] + gl_bufpos_
      );
      unbind(target_);

      gl_bufpos_ = back_;
    }
  }
}

template<Numeric T>
template<typename InputIt>
void VecBuffer<T>::add_(InputIt begin, InputIt end) {
  std::size_t size = std::distance(begin, end);
  if (fill_reverse_) {
    while (front_ + 1 <= size) {
      front_ += data_.size();
      back_ += data_.size();
      data_.reserve(data_.size() * 2);
      std::copy(data_.begin(), data_.end(), std::back_inserter(data_));
    }
    front_ -= size;
    std::copy(begin, end, data_.begin() + front_);

  } else {
    while (back_ + size >= data_.size())
      data_.resize(data_.size() * 2);
    std::copy(begin, end, data_.begin() + back_);
    back_ += size;
  }
}

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_VEC_BUFFER_HPP
