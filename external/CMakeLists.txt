add_library(myco_externals STATIC
        external/imgui/backends/imgui_impl_glfw.h
        external/imgui/backends/imgui_impl_opengl3.h
        external/imgui/backends/imgui_impl_opengl3_loader.h
        external/imgui/imconfig.h
        external/imgui/imgui.h
        external/imgui/imgui_internal.h
        external/imgui/imstb_rectpack.h
        external/imgui/imstb_textedit.h
        external/imgui/imstb_truetype.h

        external/implot/implot.h
        external/implot/implot_internal.h

        external/pcg-cpp/include/pcg_extras.hpp
        external/pcg-cpp/include/pcg_random.hpp
        external/pcg-cpp/include/pcg_uint128.hpp

        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp

        external/implot/implot.cpp
        external/implot/implot_demo.cpp
        external/implot/implot_items.cpp

        external/stb/stb_image.h
        external/stb/stb_image_resize.h
        external/stb/stb_image_write.h)

# Glad 2 downloads khrplatform.h with a Mac format,
# and the warning pops up everywhere
if(MSVC)
    target_compile_options(myco_externals PUBLIC /wd4335)
endif(MSVC)

add_subdirectory(external/fmt)

add_subdirectory(external/glad/cmake)
glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

add_subdirectory(external/glm)

set(LIBNYQUIST_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(external/libnyquist)

set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/openal-soft)

add_subdirectory(external/rapidcsv)

set(RE2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(external/re2)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory(external/spdlog)

#set(UUID_SYSTEM_GENERATOR ON CACHE BOOL "" FORCE)
add_subdirectory(external/stduuid)

target_include_directories(myco_externals PUBLIC
        external/imgui/backends
        external/imgui
        external/implot
        external/pcg-cpp/include
        external/stb)

target_link_libraries(myco_externals PUBLIC
        fmt::fmt
        glad_gl_core_mx_33
        glfw
        glm::glm
        libnyquist
        OpenAL::OpenAL
        rapidcsv
        re2::re2
        spdlog::spdlog_header_only
        stduuid)