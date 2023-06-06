cmake_minimum_required(VERSION 3.20...3.25)
project(baphy VERSION 0.1
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
add_library(baphy STATIC ${baphy_INCLUDE_FILES} ${baphy_SRC_FILES})

if (MSVC)
    target_compile_definitions(baphy PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()

target_include_directories(baphy PUBLIC include)

include(cmake/get_thirdparty.cmake)
target_link_libraries(baphy PUBLIC baphy_thirdparty)

if (WIN32)
    target_link_libraries(baphy PUBLIC imm32.lib Dwmapi.lib)
endif (WIN32)

add_subdirectory(example)
