#pragma once

#include <filesystem>

namespace myco {

const std::filesystem::path DATA = std::filesystem::path(__FILE__)
    .remove_filename()
    .parent_path()
    .parent_path()
    .parent_path()
    .parent_path()
    / "data";

} // namespace myco
