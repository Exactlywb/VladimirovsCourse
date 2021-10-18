cmake_minimum_required (VERSION 3.0)                                                                            # VERSION

project (Triangles VERSION 1.0)                                                                                 # PROJECT TITLE

set (SOURCES main.cpp ./source/triangle.cpp ./source/octree.cpp ./source/vector.cpp ./source/common.cpp ./source/intersect.cpp)      # SOURCES *.cpp
set (HEADERS include/vector.hpp include/octree.hpp include/triangle.hpp include/common.hpp)                   # HEADERS *.hpp

set (COMPILER_SETTINGS -Wall -Wextra -O2)                                                                       # COMPILER SETTINGS

add_compile_options (${COMPILER_SETTINGS})
add_executable (triangles ${SOURCES})                                                                           #
