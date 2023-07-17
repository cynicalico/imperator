cmake_minimum_required(VERSION 3.20...3.25)
project(baphy VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

include(cmake/get_cpm.cmake)
include(cmake/setup_ccache.cmake)

set(CMAKE_CXX_STANDARD 23)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(baphy STATIC ${baphy_INCLUDE_FILES} ${baphy_SRC_FILES})

if (MSVC)
    target_compile_definitions(baphy PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()
target_compile_definitions(baphy PUBLIC IMGUI_USER_CONFIG="baphy/baphy_imconfig.hpp")

target_include_directories(baphy PUBLIC include)

include(cmake/get_thirdparty.cmake)
target_link_libraries(baphy PUBLIC baphy_thirdparty)
if (WIN32)
    # imm32.lib -- IME support for imgui
    # Dwmapi.lib -- Dark mode titlebar
    # opengl32.lib -- We are loading the core profile, so it is not linked by glad2, but
    #                 we need it for the wgl extension functions
    target_link_libraries(baphy PUBLIC imm32.lib Dwmapi.lib opengl32.lib)
else ()
    # vsync control in linux requires the X11 headers
    find_package(X11 REQUIRED)
    target_link_libraries(baphy PUBLIC X11::X11)
endif (WIN32)

add_subdirectory(example)

if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/scratch)
    add_subdirectory(scratch)
endif ()