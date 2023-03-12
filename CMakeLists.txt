cmake_minimum_required(VERSION 3.12...3.25)
project(myco VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

option(MYCO_BUILD_EXAMPLES "Build example programs" ON)

set(MYCO_SPDLOG_LEVEL "SPDLOG_LEVEL_DEBUG" CACHE STRING "spdlog log level")

if(MSVC)
    set(CMAKE_CXX_STANDARD 26)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(myco STATIC ${MYCO_INCLUDE_FILES} ${MYCO_SRC_FILES})

target_compile_definitions(myco PUBLIC
        IMGUI_USER_CONFIG="myco/myco_imconfig.hpp"
        SPDLOG_ACTIVE_LEVEL=${MYCO_SPDLOG_LEVEL})

target_include_directories(myco PUBLIC include)

include(external/CMakeLists.txt)
target_link_libraries(myco PUBLIC myco_externals)

# We need this for linking ImGui properly and to set the title bar color
# to match the current windows theme
if(WIN32)
    target_link_libraries(myco PUBLIC imm32.lib Dwmapi.lib)
endif(WIN32)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(myco PUBLIC OpenMP::OpenMP_CXX)
endif()

if(MYCO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/scratch" AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/scratch")
    add_subdirectory(scratch)
endif()
