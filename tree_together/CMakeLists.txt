#
# Tree implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Ivan Ivanov, Vladislav Belov, Frolov Daniil
#

cmake_minimum_required(VERSION 3.8)

project("Tree")

set(EXEC_FILE "tree")
set(TEST_FILE "test_tree")

set(HEADERS

    include/tree.hpp

)

set(SOURCES

    sources/main.cpp
    sources/tree.cpp

)

set(TEST_SOURCES

    sources/test.cpp

)

#executable files
add_executable(${EXEC_FILE} ${SOURCES})
add_executable(${TEST_FILE} ${TEST_SOURCES})

target_include_directories(${EXEC_FILE} PRIVATE include)
target_include_directories(${TEST_FILE} PRIVATE include)

#compiler settings
add_compile_options(

    -Wall
    -Wextra
    -Wpedantic

    -Werror

    -Wsign-promo
    -Wsign-conversion
    -Wdouble-promotion

    -Wfloat-equal
    -Wbool-compire

    -Wdiv-by-zero

)
