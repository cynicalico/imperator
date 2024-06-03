#include "imperator/util/io.h"

#define STB_IMAGE_IMPLEMENTATION

#include "imperator/util/log.h"
#include "stb_image.h"
#include <fstream>

namespace imp {
ImageData::ImageData(const std::filesystem::path &path, int desired_channels) {
    int w, h, comp;
    auto data = stbi_load(path.string().c_str(), &w, &h, &comp, desired_channels);
    if (!data)
        IMPERATOR_LOG_ERROR("Failed to load image data '{}': {}", path.string(), stbi_failure_reason());
    else {
        w_ = w;
        h_ = h;
        comp_ = comp;
        bytes_ = std::vector(data, data + (w_ * h_ * comp_));
        stbi_image_free(data);
    }
}

ImageData::ImageData(std::size_t w, std::size_t h, std::size_t channels)
        : w_(w),
          h_(h),
          comp_(channels) { bytes_ = std::vector<stbi_uc>(w * h * channels, 0); }

ImageData::~ImageData() { bytes_.clear(); }

ImageData::ImageData(ImageData &&other) noexcept
        : bytes_(other.bytes_),
          w_(other.w_),
          h_(other.h_),
          comp_(other.comp_) {
    other.bytes_.clear();
    other.w_ = 0;
    other.h_ = 0;
    other.comp_ = 0;
}

ImageData &ImageData::operator=(ImageData &&other) noexcept {
    if (this != &other) {
        bytes_.clear();

        bytes_ = other.bytes_;
        w_ = other.w_;
        h_ = other.h_;
        comp_ = other.comp_;

        other.bytes_.clear();
        other.w_ = 0;
        other.h_ = 0;
        other.comp_ = 0;
    }
    return *this;
}

stbi_uc *ImageData::bytes() { return &bytes_[0]; }

int ImageData::w() const { return w_; }

int ImageData::h() const { return h_; }

int ImageData::comp() const { return comp_; }

GLFWimage ImageData::glfw_image() { return {w_, h_, &bytes_[0]}; }

stbi_uc &ImageData::operator[](std::size_t index) { return bytes_[index]; }

const stbi_uc &ImageData::operator[](std::size_t index) const { return bytes_[index]; }

stbi_uc &ImageData::operator()(std::size_t r, std::size_t c, std::size_t component) {
    return bytes_[(r * w_ * comp_) + (c * comp_) + component];
}

const stbi_uc &ImageData::operator()(std::size_t r, std::size_t c, std::size_t component) const {
    return bytes_[(r * w_ * comp_) + (c * comp_) + component];
}
} // namespace imp
