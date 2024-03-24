CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 10.2.1
)

CPMAddPackage(
        NAME glad2
        GITHUB_REPOSITORY Dav1dde/glad
        VERSION 2.0.5
        DOWNLOAD_ONLY YES
)
add_subdirectory(${glad2_SOURCE_DIR}/cmake ${glad2_BINARY_DIR})
if (WIN32)
    glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3 wgl=1.0)
else ()
    glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3 glx=1.4)
endif ()

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.9
        OPTIONS "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_EXAMPLES OFF" "GLFW_BULID_DOCS OFF"
)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 1.0.1
        OPTIONS "GLM_ENABLE_CXX_20 ON"
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.13.0
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

target_include_directories(imperator PUBLIC
        thirdparty/stb
        thirdparty/pcg)

if (MSVC)
    target_compile_definitions(imperator_thirdparty INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()

target_link_libraries(imperator_thirdparty INTERFACE
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        spdlog::spdlog
        stduuid)
