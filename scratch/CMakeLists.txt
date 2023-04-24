# Check this directory for files and build them with capra as a dependency
# This is probably very delicate, it's a better alternative to CLion's built-in
#   scratch files imo and allows more rapid prototyping (plus you get access
#   to things like {fmt} which is just objectively better than streams)

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

    add_executable("scratch_${EXECUTABLE_NAME}" ${SRC_FILENAME})
    target_link_libraries("scratch_${EXECUTABLE_NAME}" PUBLIC myco)
    target_compile_features("scratch_${EXECUTABLE_NAME}" PUBLIC cxx_std_23)
endfunction()

file(GLOB SRC_FILES *.cpp)
set(SRC_FILE_LIST "")

message(CHECK_START "Looking for scratch files")
list(APPEND CMAKE_MESSAGE_INDENT "    ")
foreach (arg IN LISTS SRC_FILES)
    build_executable(${arg} SRC_FILE_LIST)
endforeach ()
list(POP_BACK CMAKE_MESSAGE_INDENT)

list(LENGTH SRC_FILE_LIST SRC_FILES_LIST_LEN)
if (${SRC_FILES_LIST_LEN} GREATER 0)
    message(CHECK_PASS "Scratch files found")
else ()
    message(CHECK_PASS "No scratch files")
endif ()
