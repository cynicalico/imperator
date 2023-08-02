# Check this directory for files and build them with baphy as a dependency

function(build_executable SRC_FILENAME SRC_FILE_LIST TYPE_STR)
    cmake_path(GET SRC_FILENAME STEM EXECUTABLE_NAME)
    add_executable(${TYPE_STR}_${EXECUTABLE_NAME} ${SRC_FILENAME})
    target_link_libraries(${TYPE_STR}_${EXECUTABLE_NAME} PUBLIC baphy)
    target_compile_features(${TYPE_STR}_${EXECUTABLE_NAME} PUBLIC cxx_std_23)
endfunction()

function(auto_targets BASE_DIR TYPE_STR)
    file(GLOB_RECURSE SRC_FILES "${BASE_DIR}/*.cpp")
    set(SRC_FILE_LIST "")

    message(CHECK_START "Looking for ${TYPE_STR} files")
    list(APPEND CMAKE_MESSAGE_INDENT "    ")
    foreach (arg IN LISTS SRC_FILES)
        message(STATUS ${arg})
        set(SRC_FILE_LIST ${SRC_FILE_LIST} ${arg})
        build_executable(${arg} SRC_FILE_LIST ${TYPE_STR})
    endforeach ()
    list(POP_BACK CMAKE_MESSAGE_INDENT)

    list(LENGTH SRC_FILE_LIST SRC_FILES_LIST_LEN)
    if (${SRC_FILES_LIST_LEN} GREATER 0)
        message(CHECK_PASS "${TYPE_STR} files found")
    else ()
        message(CHECK_PASS "No ${TYPE_STR} files")
    endif ()
endfunction()
