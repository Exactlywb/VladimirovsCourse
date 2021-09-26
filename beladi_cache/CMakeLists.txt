cmake_minimum_required (VERSION 3.0)        # VERSION

project (Frolov_Beladi_cache VERSION 2.0)       # PROJECT TITLE

set (SOURCES main.cpp)                      # SOURCES *.cpp

set (COMPILER_SETTINGS -Wall -Wextra -O2) # COMPILER SETTINGS

add_compile_options (${COMPILER_SETTINGS})
add_executable (main ${SOURCES})            #
