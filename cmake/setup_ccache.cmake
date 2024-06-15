find_program(CCACHE_TOOL_PATH ccache)

if (NOT WIN32 AND USE_CCACHE AND CCACHE_TOOL_PATH)
    message(STATUS "Using ccache (${CCACHE_TOOL_PATH}) (via wrapper).")
    # see https://github.com/TheLartians/Ccache.cmake enables CCACHE support through
    # the USE_CCACHE flag possible values are: YES, NO or equivalent
    CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.5")
elseif (WIN32 AND USE_CCACHE AND CCACHE_TOOL_PATH)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_TOOL_PATH} CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_TOOL_PATH} CACHE STRING "" FORCE)
    message(STATUS "Using ccache (${CCACHE_TOOL_PATH}).")
endif ()
