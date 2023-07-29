#ifndef BAPHY_UTIL_IO_H
#define BAPHY_UTIL_IO_H

#include "GLFW/glfw3.h"
#include "nlohmann/json.hpp"
#include "stb_image.h"
#include <filesystem>
#include <optional>

namespace baphy {

class ImageData {
public:
  explicit ImageData(const std::filesystem::path &path, int desired_channels = 0);
  ~ImageData();

  ImageData(const ImageData &) = delete;
  ImageData &operator=(const ImageData &) = delete;

  ImageData(ImageData &&other) noexcept;
  ImageData &operator=(ImageData &&other) noexcept;

  stbi_uc &operator[](int index);
  const stbi_uc &operator[](int index) const;

  stbi_uc *bytes();

  int w();
  int h();
  int comp();

  GLFWimage glfw_image();

private:
  stbi_uc *bytes_{nullptr};

  int w_{0};
  int h_{0};
  int comp_{0};
};

const std::filesystem::path DATA_FOLDER =
    std::filesystem::path(__FILE__)
        .remove_filename()
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
    / "data";

std::optional<nlohmann::json> read_json(const std::filesystem::path &path);

} // namespace baphy

#endif//BAPHY_UTIL_IO_H
