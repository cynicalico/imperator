# Check this directory for files and build them with baphomet as a dependency

function(build_executable SRC_FILENAME SRC_FILE_LIST)
    string(FIND ${SRC_FILENAME} "/" START_EXECUTABLE_NAME REVERSE)
    string(FIND ${SRC_FILENAME} ".cpp" END_EXECUTABLE_NAME REVERSE)
    if (${END_EXECUTABLE_NAME} EQUAL -1)
        return()
    else ()
        message(STATUS ${SRC_FILENAME})
        set(SRC_FILE_LIST ${SRC_FILE_LIST} ${SRC_FILENAME} PARENT_SCOPE)
    endif ()

    math(EXPR START_POS "${START_EXECUTABLE_NAME} + 1")
    math(EXPR LEN "${END_EXECUTABLE_NAME} - ${START_EXECUTABLE_NAME} - 1")
    string(SUBSTRING ${SRC_FILENAME} ${START_POS} ${LEN} EXECUTABLE_NAME)

    add_executable("example_${EXECUTABLE_NAME}" ${SRC_FILENAME})
    target_link_libraries("example_${EXECUTABLE_NAME}" PUBLIC baphomet)
    target_compile_features("example_${EXECUTABLE_NAME}" PUBLIC cxx_std_23)
endfunction()

file(GLOB SRC_FILES *.cpp **/*.cpp)
set(SRC_FILE_LIST "")

message(CHECK_START "Looking for example files")
list(APPEND CMAKE_MESSAGE_INDENT "    ")
foreach (arg IN LISTS SRC_FILES)
    build_executable(${arg} SRC_FILE_LIST)
endforeach ()
list(POP_BACK CMAKE_MESSAGE_INDENT)

list(LENGTH SRC_FILE_LIST SRC_FILES_LIST_LEN)
if (${SRC_FILES_LIST_LEN} GREATER 0)
    message(CHECK_PASS "Example files found")
else ()
    message(CHECK_PASS "No example files")
endif ()
