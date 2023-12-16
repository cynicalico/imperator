#ifndef IMPERATOR_UTIL_IO_HPP
#define IMPERATOR_UTIL_IO_HPP

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "nlohmann/json.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <filesystem>
#include <optional>

namespace imp {
class ImageData {
public:
  explicit ImageData(const std::filesystem::path& path, int desired_channels = 0);
  ImageData(std::size_t w, std::size_t h, std::size_t channels = 4);

  ~ImageData();

  ImageData(const ImageData&) = delete;
  ImageData& operator=(const ImageData&) = delete;

  ImageData(ImageData&& other) noexcept;
  ImageData& operator=(ImageData&& other) noexcept;

  stbi_uc& operator[](std::size_t index);
  const stbi_uc& operator[](std::size_t index) const;

  stbi_uc& operator()(std::size_t r, std::size_t c, std::size_t component);
  const stbi_uc& operator()(std::size_t r, std::size_t c, std::size_t component) const;

  stbi_uc* bytes();

  int w() const;
  int h() const;
  int comp() const;

  GLFWimage glfw_image();

private:
  std::vector<stbi_uc> bytes_{};

  int w_{0};
  int h_{0};
  int comp_{0};
};

// TODO: Make this configurable
const std::filesystem::path DATA_FOLDER = std::filesystem::current_path() / "data";

std::optional<nlohmann::json> read_json(const std::filesystem::path& path);
} // namespace imp

#endif//IMPERATOR_UTIL_IO_HPP
