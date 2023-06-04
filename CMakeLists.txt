cmake_minimum_required(VERSION 3.12...3.25)
project(baphomet VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

include(cmake/get_cpm.cmake)
include(cmake/setup_ccache.cmake)

if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else ()
    set(CMAKE_CXX_STANDARD 20)
endif ()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(baphomet STATIC ${baphomet_INCLUDE_FILES} ${baphomet_SRC_FILES})

if (MSVC)
    target_compile_definitions(baphomet PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()
target_compile_definitions(baphomet PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

target_include_directories(baphomet PUBLIC include)

include(cmake/get_thirdparty.cmake)
target_link_libraries(baphomet PUBLIC baphomet_thirdparty)

if (WIN32)
    target_link_libraries(baphomet PUBLIC imm32.lib Dwmapi.lib)
endif (WIN32)

add_subdirectory(example)
