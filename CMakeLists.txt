cmake_minimum_required(VERSION 3.20...3.25)
project(imp VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

# We need Jinja2 for Glad 2 to work
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND python -m pip show Jinja2 RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
if(NOT ${EXIT_CODE} EQUAL 0)
    message(FATAL_ERROR "The Jinja2 Python package is not installed. Please install it using the following command: \"python -m pip install jinja2\".")
endif()

# This is kind of a workaround to handle the OpenAL DLL... not sure what the real solution is
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(cmake/get_cpm.cmake)
include(cmake/setup_ccache.cmake)

set(CMAKE_CXX_STANDARD 23)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /Zc:preprocessor") # Disable RTTI
endif ()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(imp STATIC ${imp_INCLUDE_FILES} ${imp_SRC_FILES})

if (MSVC)
    target_compile_definitions(imp PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()
target_compile_definitions(imp PUBLIC IMGUI_USER_CONFIG="imp/imp_imconfig.hpp")

target_include_directories(imp PUBLIC include)

include(cmake/get_thirdparty.cmake)
target_link_libraries(imp PUBLIC imp_thirdparty)
if (WIN32)
    # ntdll -- Windows version info
    # imm32.lib -- IME support for imgui
    # Dwmapi.lib -- Dark mode titlebar
    # opengl32.lib -- We are loading the core profile, so it is not linked by glad2, but
    #                 we need it for the wgl extension functions
    target_link_libraries(imp PUBLIC ntdll imm32.lib Dwmapi.lib opengl32.lib)
else ()
    # vsync control in linux requires the X11 headers
    find_package(X11 REQUIRED)
    target_link_libraries(imp PUBLIC X11::X11)
endif (WIN32)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(imp PUBLIC OpenMP::OpenMP_CXX)
endif()

add_subdirectory(example)

if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/scratch)
    add_subdirectory(scratch)
endif ()