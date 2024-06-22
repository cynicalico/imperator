# We need Jinja2 for Glad 2 to work
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND ${Python3_EXECUTABLE} -m pip show Jinja2 RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
if(NOT ${EXIT_CODE} EQUAL 0)
    message(FATAL_ERROR "The Jinja2 Python package is not installed. Please install it using the following command: \"${Python3_EXECUTABLE} -m pip install jinja2\".")
endif()
CPMAddPackage(
        NAME glad2
        GITHUB_REPOSITORY Dav1dde/glad
        VERSION 2.0.6
        DOWNLOAD_ONLY YES
)
add_subdirectory(${glad2_SOURCE_DIR}/cmake ${glad2_BINARY_DIR})
if (WIN32)
    glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3 wgl=1.0)
else ()
    glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3 glx=1.4 egl=1.5)
endif ()

add_library(imperator_thirdparty INTERFACE
        thirdparty/stb/stb_image.h
        thirdparty/stb/stb_image_write.h

        thirdparty/pcg/pcg_extras.hpp
        thirdparty/pcg/pcg_random.hpp
        thirdparty/pcg/pcg_uint128.hpp)

target_include_directories(imperator PUBLIC
        thirdparty/stb
        thirdparty/pcg)

if (MSVC)
    target_compile_definitions(imperator_thirdparty INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif (UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(imperator_thirdparty INTERFACE ${X11_LIBRARIES})
endif ()

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)

target_link_libraries(imperator_thirdparty INTERFACE
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        re2::re2
        spdlog::spdlog
        stduuid)
