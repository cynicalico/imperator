#ifndef MYCO_UTIL_IO_H
#define MYCO_UTIL_IO_H

#include <filesystem>

const std::filesystem::path DATA =
        std::filesystem::path(__FILE__)
                .remove_filename()
                .parent_path()
                .parent_path()
                .parent_path()
                .parent_path()
        / "data";

#endif//MYCO_UTIL_IO_H
