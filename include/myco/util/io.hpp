#pragma once

#include <filesystem>
#include <string>

namespace myco {

const std::filesystem::path DATA = std::filesystem::path(__FILE__)
    .remove_filename()
    .parent_path()
    .parent_path()
    .parent_path()
    .parent_path()
    / "data";

std::string read_file_to_string(const std::filesystem::path &path);

} // namespace myco
