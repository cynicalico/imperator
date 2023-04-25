cmake_minimum_required(VERSION 3.12...3.25)
project(myco VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

set(CPM_SOURCE_CACHE .cache/CPM)
include(cmake/get_cpm.cmake)
include(cmake/setup_ccache.cmake)

if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else ()
    set(CMAKE_CXX_STANDARD 20)
endif ()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(myco STATIC ${MYCO_INCLUDE_FILES} ${MYCO_SRC_FILES})

target_include_directories(myco PUBLIC include)

include(cmake/get_thirdparty.cmake)
target_link_libraries(myco PUBLIC myco_thirdparty)

if (WIN32)
    target_link_libraries(myco PUBLIC imm32.lib Dwmapi.lib)
endif (WIN32)

if (EXISTS "${CMAKE_SOURCE_DIR}/scratch" AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/scratch")
    add_subdirectory(scratch)
endif ()
