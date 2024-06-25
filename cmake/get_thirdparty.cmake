CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 10.2.1
)

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

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.4
        OPTIONS "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_EXAMPLES OFF" "GLFW_BULID_DOCS OFF"
)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 1.0.1
        OPTIONS "GLM_ENABLE_CXX_20 ON"
)

CPMAddPackage(
        NAME sndfile
        GITHUB_REPOSITORY libsndfile/libsndfile
        GIT_TAG 1.2.2
)

CPMAddPackage(
        NAME openal-soft
        GITHUB_REPOSITORY kcat/openal-soft
        GIT_TAG 3c9cb48bc38dcc77560b0d2b7cbff240c98a260d
        OPTIONS
            "ALSOFT_BACKEND_WINMM OFF"
            "ALSOFT_REQUIRE_WASAPI ON"
            "ALSOFT_BACKEND_DSOUND OFF"
            "ALSOFT_BACKEND_WAVE OFF"
)

# For re2, we don't actually link it
CPMAddPackage(
        NAME abseil-cpp
        GITHUB_REPOSITORY abseil/abseil-cpp
        GIT_TAG 20240116.2
        OPTIONS "ABSL_PROPAGATE_CXX_STD ON" "ABSL_ENABLE_INSTALL ON"
)

CPMAddPackage(
        NAME re2
        GITHUB_REPOSITORY google/re2
        GIT_TAG 2024-05-01
        OPTIONS "RE2_BUILD_TESTING OFF"
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.14.1
        OPTIONS "SPDLOG_FMT_EXTERNAL ON"
)

CPMAddPackage(
        NAME stduuid
        GITHUB_REPOSITORY mariusbancila/stduuid
        VERSION 1.2.3
)

add_library(imperator_thirdparty INTERFACE
        thirdparty/stb/stb_image.h
        thirdparty/stb/stb_image_write.h

        thirdparty/pcg/pcg_extras.hpp
        thirdparty/pcg/pcg_random.hpp
        thirdparty/pcg/pcg_uint128.hpp)

if (MSVC)
    target_compile_definitions(imperator_thirdparty INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif (UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(imperator_thirdparty INTERFACE ${X11_LIBRARIES})
endif ()

target_include_directories(imperator_thirdparty INTERFACE
        thirdparty/stb
        thirdparty/pcg)

target_link_libraries(imperator_thirdparty INTERFACE
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        OpenAL::OpenAL
        re2::re2
        SndFile::sndfile
        spdlog::spdlog
        stduuid)
