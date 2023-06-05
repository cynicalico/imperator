# Check this directory for files and build them with baphy as a dependency

include(../cmake/auto_targets.cmake)
auto_targets(${CMAKE_CURRENT_LIST_DIR} "example")
