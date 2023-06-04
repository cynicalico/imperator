#ifndef BAPHY_UTIL_IO_H
#define BAPHY_UTIL_IO_H

#include <filesystem>

const std::filesystem::path BAPHY_DATA =
    std::filesystem::path(__FILE__)
        .remove_filename()
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
    / "data";

#endif//BAPHY_UTIL_IO_H
