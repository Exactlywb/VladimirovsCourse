#
# Matrix implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Frolov Daniil
#

cmake_minimum_required(VERSION 3.8)

project("Matrix")

set (GTEST_ROOT ${CMAKE_SOURCE_DIR}/ExternalLibs/gTest)
find_package(GTest REQUIRED)

set(EXEC_FILE "matrix")
set(TEST_FILE "test_matrix")

set(HEADERS

    include/matrix.hpp
    include/common.hpp

)

set(SOURCES

    sources/main.cpp
    sources/common.cpp

)

set(TEST_SOURCES

    sources/test.cpp
    sources/common.cpp

)

#executable files
add_executable(${EXEC_FILE} ${SOURCES})
add_executable(${TEST_FILE} ${TEST_SOURCES})

target_include_directories(${EXEC_FILE} PRIVATE include)

target_link_libraries(${TEST_FILE} PRIVATE "${GTEST_LIBRARIES}" "pthread")
target_include_directories(${TEST_FILE} PRIVATE include "${GTEST_INCLUDE_DIRS}")

#compiler settings
add_compile_options(

    -Wall
    -Wextra
    -Wpedantic

    -Wsign-promo
    -Wsign-conversion
    -Wdouble-promotion

    -Wfloat-equal
    -Wbool-compire

    -Wdiv-by-zero

    -O2

)