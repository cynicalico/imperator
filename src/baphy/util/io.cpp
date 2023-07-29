#include "baphy/util/io.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "baphy/util/log.hpp"
#include "stb_image.h"
#include <fstream>

namespace baphy {

std::optional<nlohmann::json> read_json(const std::filesystem::path &path) {
  std::ifstream ifs(path);
  if (!ifs.is_open())
    return std::nullopt;

  std::optional<nlohmann::json> j{};
  try {
     j = nlohmann::json::parse(ifs);
  } catch (const nlohmann::json::exception &e) {
    BAPHY_LOG_ERROR("Failed to parse json '{}': {}", path.string(), e.what());
  }

  return j;
}

ImageData::ImageData(const std::filesystem::path &path, int desired_channels) {
  bytes_ = stbi_load(path.string().c_str(), &w_, &h_, &comp_, desired_channels);
  if (!bytes_)
    BAPHY_LOG_ERROR("Failed to load image data '{}': {}", path.string(), stbi_failure_reason());
}

ImageData::~ImageData() {
  stbi_image_free(bytes_);
}

ImageData::ImageData(ImageData &&other) noexcept : bytes_(other.bytes_), w_(other.w_), h_(other.h_), comp_(other.comp_) {
  other.bytes_ = nullptr;
  other.w_ = 0;
  other.h_ = 0;
  other.comp_ = 0;
}

ImageData &ImageData::operator=(ImageData &&other) noexcept {
  if (this != &other) {
    stbi_image_free(bytes_);

    bytes_ = other.bytes_;
    w_ = other.w_;
    h_ = other.h_;
    comp_ = other.comp_;

    other.bytes_ = nullptr;
    other.w_ = 0;
    other.h_ = 0;
    other.comp_ = 0;
  }
  return *this;
}

stbi_uc *ImageData::bytes() {
  return bytes_;
}

int ImageData::w() {
  return w_;
}

int ImageData::h() {
  return h_;
}

int ImageData::comp() {
  return comp_;
}

GLFWimage ImageData::glfw_image() {
  return {w_, h_, bytes_};
}

stbi_uc &ImageData::operator[](int index) {
  return bytes_[index];
}

const stbi_uc &ImageData::operator[](int index) const {
  return bytes_[index];
}

} // namespace baphy
