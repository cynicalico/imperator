CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 10.1.1
)

CPMAddPackage(
        NAME glad2
        GITHUB_REPOSITORY Dav1dde/glad
        VERSION 2.0.4
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
        GIT_TAG 3.3.8
        OPTIONS
            "GLFW_BUILD_TESTS OFF"
            "GLFW_BUILD_EXAMPLES OFF"
            "GLFW_BULID_DOCS OFF"
)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 0.9.9.8
)

CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG d2291df55190e2f070af2635863f47a96d378a52
        DOWNLOAD_ONLY YES
)

CPMAddPackage(
        NAME implot
        GITHUB_REPOSITORY epezent/implot
        GIT_TAG 18758e237e8906a97ddf42de1e75793526f30ce9
        DOWNLOAD_ONLY
)

CPMAddPackage(
        NAME json
        GITHUB_REPOSITORY nlohmann/json
        VERSION 3.11.2
)

CPMAddPackage(
        NAME nfd
        GITHUB_REPOSITORY btzy/nativefiledialog-extended
        VERSION 1.1.0
)

CPMAddPackage(
        NAME pcg-cpp
        GITHUB_REPOSITORY imneme/pcg-cpp
        GIT_TAG 428802d1a5634f96bcd0705fab379ff0113bcf13
        DOWNLOAD_ONLY YES
)

CPMAddPackage(
        NAME rapidcsv
        GITHUB_REPOSITORY d99kris/rapidcsv
        VERSION 8.80
)

# RE2 requires abseil to build
CPMAddPackage(
        NAME abseil-cpp
        GITHUB_REPOSITORY abseil/abseil-cpp
        GIT_TAG 20230802.1
        OPTIONS
        "ABSL_PROPAGATE_CXX_STD ON"
        "ABSL_ENABLE_INSTALL ON"
)

CPMAddPackage(
        NAME re2
        GITHUB_REPOSITORY google/re2
        GIT_TAG 2023-09-01
        OPTIONS
            "RE2_BUILD_TESTING OFF"
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.12.0
        OPTIONS
            "SPDLOG_FMT_EXTERNAL ON"
)

CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY nothings/stb
        GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
        DOWNLOAD_ONLY YES
)

CPMAddPackage(
        NAME stduuid
        GITHUB_REPOSITORY mariusbancila/stduuid
        VERSION 1.2.3
)

add_library(baphy_thirdparty STATIC
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imstb_rectpack.h
        ${imgui_SOURCE_DIR}/imstb_textedit.h
        ${imgui_SOURCE_DIR}/imstb_truetype.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h

        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

        ${implot_SOURCE_DIR}/implot.h
        ${implot_SOURCE_DIR}/implot_internal.h

        ${implot_SOURCE_DIR}/implot.cpp
        ${implot_SOURCE_DIR}/implot_demo.cpp
        ${implot_SOURCE_DIR}/implot_items.cpp

        ${pcg-cpp_SOURCE_DIR}/include/pcg_extras.hpp
        ${pcg-cpp_SOURCE_DIR}/include/pcg_random.hpp
        ${pcg-cpp_SOURCE_DIR}/include/pcg_uint128.hpp

        ${stb_SOURCE_DIR}/stb_image.h
        ${stb_SOURCE_DIR}/stb_image_write.h
        ${stb_SOURCE_DIR}/stb_image_resize.h)

if (MSVC)
    target_compile_definitions(baphy_thirdparty PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)
else ()
    set(NFD_PORTAL ON CACHE STRING "" FORCE)
endif ()

target_include_directories(baphy_thirdparty PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${implot_SOURCE_DIR}
        ${pcg-cpp_SOURCE_DIR}/include
        ${stb_SOURCE_DIR})

target_link_libraries(baphy_thirdparty PUBLIC
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        nfd
        nlohmann_json::nlohmann_json
        rapidcsv
        re2::re2
        spdlog::spdlog
        stduuid)
