#include "baphy/util/io.hpp"

#include "baphy/util/log.hpp"
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

} // namespace baphy
