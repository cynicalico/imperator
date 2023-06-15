#ifndef BAPHY_UTIL_IO_H
#define BAPHY_UTIL_IO_H

#include <filesystem>

namespace baphy {

const std::filesystem::path DATA_FOLDER =
    std::filesystem::path(__FILE__)
        .remove_filename()
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
    / "data";

} // namespace baphy

#endif//BAPHY_UTIL_IO_H
