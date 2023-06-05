CPMAddPackage(
        NAME abseil
        GITHUB_REPOSITORY abseil/abseil-cpp
        GIT_TAG 419b54d4870e31ae2442b7f453c7b8d5b8c08026
        OPTIONS
        "ABSL_PROPAGATE_CXX_STD ON"
)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 0de789cf294ae59582e7372884bbdb7158e90b8d
)

CPMAddPackage(
        NAME glad2
        GITHUB_REPOSITORY Dav1dde/glad
        GIT_TAG v2.0.4
        DOWNLOAD_ONLY YES
)
add_subdirectory(${glad2_SOURCE_DIR}/cmake ${glad2_BINARY_DIR})
glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)

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
        GIT_TAG efec5db081e3aad807d0731e172ac597f6a39447
)

CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG d2291df55190e2f070af2635863f47a96d378a52
        DOWNLOAD_ONLY YES
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
        GIT_TAG 19946a86c255d430c4af84d3bfece36cf008feef
)

CPMAddPackage(
        NAME re2
        GITHUB_REPOSITORY google/re2
        GIT_TAG 11073deb73b3d01018308863c0bcdfd0d51d3e70
        OPTIONS
        "RE2_BUILD_TESTING OFF"
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG 0ca574ae168820da0268b3ec7607ca7b33024d05
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
        GIT_TAG 3afe7193facd5d674de709fccc44d5055e144d7a
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

        ${pcg-cpp_SOURCE_DIR}/include/pcg_extras.hpp
        ${pcg-cpp_SOURCE_DIR}/include/pcg_random.hpp
        ${pcg-cpp_SOURCE_DIR}/include/pcg_uint128.hpp

        ${stb_SOURCE_DIR}/stb_image.h
        ${stb_SOURCE_DIR}/stb_image_write.h
        ${stb_SOURCE_DIR}/stb_image_resize.h)

if (MSVC)
    target_compile_definitions(baphy_thirdparty PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()

target_include_directories(baphy_thirdparty PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${pcg-cpp_SOURCE_DIR}/include
        ${stb_SOURCE_DIR})

target_link_libraries(baphy_thirdparty PUBLIC
        absl::flat_hash_map
        absl::hash
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        rapidcsv
        re2::re2
        spdlog::spdlog
        stduuid)
