cmake_minimum_required (VERSION 3.0)                            			# VERSION

project (Triangles VERSION 1.0)                                 			# PROJECT TITLE

set (SOURCES main.cpp)                                          			# SOURCES *.cpp
set (HEADERS include/vector.hpp include/octotree.hpp include/triangle.hpp) 	# HEADERS *.hpp

set (COMPILER_SETTINGS -Wall -Wextra -O2)                       			# COMPILER SETTINGS

add_compile_options (${COMPILER_SETTINGS})
add_executable (triangles ${SOURCES})                                #
