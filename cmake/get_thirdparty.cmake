CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 0de789cf294ae59582e7372884bbdb7158e90b8d
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG 0ca574ae168820da0268b3ec7607ca7b33024d05
        OPTIONS
            "SPDLOG_FMT_EXTERNAL ON"
)

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
        NAME re2
        GITHUB_REPOSITORY google/re2
        GIT_TAG 11073deb73b3d01018308863c0bcdfd0d51d3e70
        OPTIONS
            "RE2_BUILD_TESTING OFF"
)

add_library(myco_thirdparty INTERFACE)
target_link_libraries(myco_thirdparty INTERFACE
                      fmt::fmt
                      glfw
                      re2::re2
                      spdlog::spdlog)
