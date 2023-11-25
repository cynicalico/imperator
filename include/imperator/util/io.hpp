#ifndef IMPERATOR_UTIL_IO_HPP
#define IMPERATOR_UTIL_IO_HPP

#include "nlohmann/json.hpp"
#include <filesystem>
#include <optional>

namespace imp {

const std::filesystem::path DATA_FOLDER =
    std::filesystem::path(__FILE__)
        .remove_filename()
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
    / "data";

std::optional<nlohmann::json> read_json(const std::filesystem::path& path);

} // namespace imp

#endif//IMPERATOR_UTIL_IO_HPP
