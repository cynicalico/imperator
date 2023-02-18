cmake_minimum_required(VERSION 3.12...3.23)
project(myco VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

option(MYCO_BUILD_EXAMPLES "Build example programs" ON)

set(MYCO_SPDLOG_LEVEL "SPDLOG_LEVEL_DEBUG" CACHE STRING "spdlog log level")

if(MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(myco STATIC ${MYCO_INCLUDE_FILES} ${MYCO_SRC_FILES} scratch/priority_v2.cpp)

target_compile_definitions(myco PUBLIC
#        IMGUI_USER_CONFIG="capra/capra_imconfig.hpp"
        SPDLOG_ACTIVE_LEVEL=${MYCO_SPDLOG_LEVEL})

target_include_directories(myco PUBLIC include)

include(external/CMakeLists.txt)
target_link_libraries(myco PUBLIC myco_externals)

if(MYCO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/scratch" AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/scratch")
    add_subdirectory(scratch)
endif()