#ifndef IMPERATOR_GL_VECBUF_H
#define IMPERATOR_GL_VECBUF_H

#include "imperator/gl/buffer.h"
#include <concepts>
#include <vector>

namespace imp {
template<Numeric T = float>
class VecBuf : public Buffer {
public:
    VecBuf(
        GfxContext &gfx,
        std::size_t initial_size,
        bool fill_reverse,
        BufTarget target,
        BufUsage usage
    );

    ~VecBuf() override = default;

    // Copy constructors don't make sense for OpenGL objects
    VecBuf(const VecBuf &) = delete;

    VecBuf &operator=(const VecBuf &) = delete;

    VecBuf(VecBuf &&other) noexcept;

    VecBuf &operator=(VecBuf &&other) noexcept;

    void clear();

    std::size_t front() const;

    std::size_t size() const;

    std::size_t capacity() const;

    void add(const std::vector<T> &new_data);

    void add(std::initializer_list<T> new_data);

    template<std::ranges::input_range R>
    void add(const R &new_data);

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

using VecBufF = VecBuf<float>;
using VecBufI = VecBuf<int>;
using VecBufU = VecBuf<unsigned int>;

template<Numeric T>
VecBuf<T>::VecBuf(
    GfxContext &gfx,
    std::size_t initial_size,
    bool fill_reverse,
    BufTarget target,
    BufUsage usage
) : Buffer(gfx),
    target_(target),
    usage_(usage),
    fill_reverse_(fill_reverse) {
    data_.resize(initial_size);
    if (fill_reverse_) {
        front_ = initial_size;
        back_ = initial_size;
    }
}

template<Numeric T>
VecBuf<T>::VecBuf(VecBuf<T> &&other) noexcept : Buffer(std::move(other)) {
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
VecBuf<T> &VecBuf<T>::operator=(VecBuf<T> &&other) noexcept {
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
void VecBuf<T>::clear() {
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
std::size_t VecBuf<T>::front() const {
    return front_;
}

template<Numeric T>
std::size_t VecBuf<T>::size() const {
    return back_ - front_;
}

template<Numeric T>
std::size_t VecBuf<T>::capacity() const {
    return data_.size();
}

template<Numeric T>
void VecBuf<T>::add(const std::vector<T> &new_data) {
    add_(new_data.begin(), new_data.end());
}

template<Numeric T>
void VecBuf<T>::add(std::initializer_list<T> new_data) {
    add_(new_data.begin(), new_data.end());
}

template<Numeric T>
template<std::ranges::input_range R>
void VecBuf<T>::add(const R &new_data) {
    add_(std::ranges::begin(new_data), std::ranges::end(new_data));
}

template<Numeric T>
void VecBuf<T>::sync() {
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
void VecBuf<T>::add_(InputIt begin, InputIt end) {
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
        while (back_ + size >= data_.size()) {
            data_.resize(data_.size() * 2);
        }
        std::copy(begin, end, data_.begin() + back_);
        back_ += size;
    }
}
} // namespace imp

#endif//IMPERATOR_GL_VECBUF_H
