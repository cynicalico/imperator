cmake_minimum_required(VERSION 3.20..3.25)
project(imperator
        VERSION 1.0
        DESCRIPTION "TBD"
        LANGUAGES C CXX)

include("cmake/get_cpm.cmake")
include("cmake/setup_ccache.cmake")

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

add_library(imperator
        "include/imperator/core/engine.h"
        "include/imperator/core/event_bus.h"
        "include/imperator/core/events.h"
        "include/imperator/core/glfw_callbacks.h"
        "include/imperator/core/glfw_wrap.h"
        "include/imperator/core/type_id.h"
        "include/imperator/ds/prio_list.h"
        "include/imperator/ds/trie.h"
        "include/imperator/module/application.h"
        "include/imperator/module/window.h"
        "include/imperator/util/averagers.h"
        "include/imperator/util/enum_bitops.h"
        "include/imperator/util/io.h"
        "include/imperator/util/log.h"
        "include/imperator/util/platform.h"
        "include/imperator/util/rnd.h"
        "include/imperator/util/time.h"
        "include/imperator/imperator.h"

        "src/imperator/core/engine.cpp"
        "src/imperator/core/glfw_callbacks.cpp"
        "src/imperator/ds/prio_list.cpp"
        "src/imperator/ds/trie.cpp"
        "src/imperator/module/application.cpp"
        "src/imperator/module/window.cpp"
        "src/imperator/util/averagers.cpp"
        "src/imperator/util/io.cpp"
        "src/imperator/util/log.cpp"
        "src/imperator/util/platform.cpp"
        "src/imperator/util/rnd.cpp"
        "src/imperator/util/time.cpp"
        "src/imperator/imperator.cpp")

target_include_directories(imperator PUBLIC "include")

target_compile_features(imperator PUBLIC cxx_std_23)
if (MSVC)
    target_compile_definitions(imperator PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_compile_options(imperator PUBLIC /utf-8 /W4)
endif ()

include(cmake/get_thirdparty.cmake)
target_link_libraries(imperator PUBLIC imperator_thirdparty)

add_subdirectory(example)