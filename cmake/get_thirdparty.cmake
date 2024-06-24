# We need Jinja2 for Glad 2 to work
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND ${Python3_EXECUTABLE} -m pip show Jinja2 RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
if (NOT ${EXIT_CODE} EQUAL 0)
    message(FATAL_ERROR "The Jinja2 Python package is not installed. Please install it using the following command: \"${Python3_EXECUTABLE} -m pip install jinja2\".")
endif ()
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

add_library(imperator_thirdparty INTERFACE)

if (MSVC)
    target_compile_definitions(imperator_thirdparty INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif (UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(imperator_thirdparty INTERFACE ${X11_LIBRARIES})
endif ()

find_package(argparse CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(stduuid CONFIG REQUIRED)

target_include_directories(imperator_thirdparty INTERFACE ${Stb_INCLUDE_DIR} ${PCG_INCLUDE_DIRS})

target_link_libraries(imperator_thirdparty INTERFACE
        argparse::argparse
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        OpenAL::OpenAL
        re2::re2
        SndFile::sndfile
        spdlog::spdlog
        stduuid)
