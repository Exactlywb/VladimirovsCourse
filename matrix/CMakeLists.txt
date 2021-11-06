#
# Matrix implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Frolov Daniil
#

cmake_minimum_required(VERSION 3.8)

project("Matrix")

set(EXEC_FILE "matrix")

set(HEADERS

    "include/matrix.hpp"

)

set(SOURCES

    "sources/main.cpp"
    "sources/matrix.cpp"

)

#executable files
add_executable(${EXEC_FILE} ${SOURCES})
target_include_directories(${EXEC_FILE} PRIVATE include)

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

)