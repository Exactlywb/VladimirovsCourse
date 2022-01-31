#
# Tree implementation (lvl_1 ---> K-th order statistic)
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Ivan Ivanov, Vladislav Belov, Frolov Daniil
#

cmake_minimum_required (VERSION 3.16)

project ("Splay Tree" VERSION 1.0)

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/sources/*.[ch]pp
        ${PROJECT_SOURCE_DIR}/sources/*.[ch]
        ${PROJECT_SOURCE_DIR}/include/*.[h]pp
        ${PROJECT_SOURCE_DIR}/include/*.[h]
    )

    add_custom_target(
        format
        COMMAND clang-format
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
endif()

#uncomment it after adding GTests
##############################################################################
# add_subdirectory(googletest)
##############################################################################

set (EXEC_FILE "splay-tree")
set (GEN_FILE  "generator")

#uncomment it after adding GTests
##############################################################################
# set (GTEST_ROOT ${CMAKE_SOURCE_DIR}/googletests/gTest)
# find_package(GTest REQUIRED)
##############################################################################

set (SOURCES
    sources/main.cpp
)

set (GENERATOR
    generator/generator.cpp
)

set (HEADERS
    include/matrix.hpp
)

add_executable(${EXEC_FILE} ${SOURCES})
add_executable(${GEN_FILE}  ${GENERATOR})

target_include_directories(${EXEC_FILE} PRIVATE include)

#uncomment it after adding GTests
##############################################################################
# target_link_libraries(${TEST_FILE} PRIVATE "${GTEST_LIBRARIES}" "pthread")
# target_include_directories(${TEST_FILE} PRIVATE headers "${GTEST_INCLUDE_DIRS}")
##############################################################################

set (COMPILER_SETTINGS
    -std=c++2a
    -Wall
    -Wextra
    -Wpedantic

    -Wsign-promo
    -Wsign-conversion
    -Wdouble-promotion

    -Wfloat-equal
    -Wbool-compare

    -Wdiv-by-zero

    -O2
)

target_compile_options(${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS})
target_compile_options(${GEN_FILE}  PRIVATE ${COMPILER_SETTINGS})