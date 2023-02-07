cmake_minimum_required(VERSION 3.12...3.23)
project(myco VERSION 0.1
        DESCRIPTION "WIP"
        LANGUAGES CXX C)

option(MYCO_BUILD_EXAMPLES "Build example programs" ON)

if(MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

include(include/CMakeLists.txt)
include(src/CMakeLists.txt)
add_library(myco STATIC ${MYCO_INCLUDE_FILES} ${MYCO_SRC_FILES})

target_include_directories(myco PUBLIC include)

include(external/CMakeLists.txt)
target_link_libraries(myco PUBLIC myco_externals)

if(MYCO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()