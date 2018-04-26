cmake_minimum_required(VERSION 3.8)

project(cilisp)

set(CMAKE_C_STANDARD 99)

SET(CMAKE_C_FLAGS "-m64 -g -O0 -D_DEBUG -Wall -Wextra -Wunreachable-code")

set(SOURCE_FILES
        ciLispSrc/ciLisp.c
        ${CMAKE_CURRENT_BINARY_DIR}/ciLispScanner.c
        ${CMAKE_CURRENT_BINARY_DIR}/ciLispParser.c
        )

include_directories(AFTER ciLispSrc)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(ciLispParser ciLispSrc/ciLisp.y ${CMAKE_CURRENT_BINARY_DIR}/ciLispParser.c VERBOSE)
FLEX_TARGET(ciLispScanner ciLispSrc/ciLisp.l ${CMAKE_CURRENT_BINARY_DIR}/ciLispScanner.c)

ADD_FLEX_BISON_DEPENDENCY(ciLispScanner ciLispParser)

add_executable(
        cilisp
        ${SOURCE_FILES}
        ${BISON_ciLispParser_OUTPUTS}
        ${FLEX_ciLispScanner_OUTPUTS}
)