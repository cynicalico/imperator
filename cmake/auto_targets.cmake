# Check a given directory for .cpp files and build them with an optional list of libs

function(build_executable SRC_FILENAME SRC_FILE_LIST TYPE_STR LIBS)
    cmake_path(GET SRC_FILENAME STEM EXECUTABLE_NAME)
    if(NOT TYPE_STR STREQUAL "")
        set(EXECUTABLE_NAME ${TYPE_STR}_${EXECUTABLE_NAME})
    endif()
    add_executable(${EXECUTABLE_NAME} ${SRC_FILENAME})
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${LIBS})
    target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_23)
endfunction()

function(auto_targets)
    cmake_parse_arguments(
            PARSE_ARGV 0
            PARSED_ARGS
            ""
            "BASE_DIR;TYPE_STR"
            "LIBS"
    )
    if(NOT PARSED_ARGS_TYPE_STR)
        set(PARSED_ARGS_TYPE_STR "")
    endif()
    if(NOT PARSED_ARGS_LIBS)
        set(PARSED_ARGS_LIBS "")
    endif()

    file(GLOB_RECURSE SRC_FILES "${PARSED_ARGS_BASE_DIR}/*.cpp")
    set(SRC_FILE_LIST "")

    message(CHECK_START "Looking for ${PARSED_ARGS_TYPE_STR} files")
    list(APPEND CMAKE_MESSAGE_INDENT "    ")
    foreach(arg IN LISTS SRC_FILES)
        message(STATUS ${arg})
        set(SRC_FILE_LIST ${SRC_FILE_LIST} ${arg})
        build_executable(${arg} SRC_FILE_LIST "${PARSED_ARGS_TYPE_STR}" "${PARSED_ARGS_LIBS}")
    endforeach()
    list(POP_BACK CMAKE_MESSAGE_INDENT)

    list(LENGTH SRC_FILE_LIST SRC_FILES_LIST_LEN)
    if(${SRC_FILES_LIST_LEN} GREATER 0)
        message(CHECK_PASS "${SRC_FILES_LIST_LEN} ${PARSED_ARGS_TYPE_STR} file(s) found")
    else()
        message(CHECK_PASS "No ${PARSED_ARGS_TYPE_STR} files")
    endif()
endfunction()
