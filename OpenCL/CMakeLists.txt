cmake_minimum_required (VERSION 3.10)
project (BitonicSort)

find_package (OpenCL REQUIRED)

set (SOURCES
    source/main.cpp
    source/app.cpp
)
add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories (${PROJECT_NAME} PRIVATE include)

target_link_libraries (${PROJECT_NAME} OpenCL::OpenCL)

set (COMPILER_SETTINGS
    -std=c++2a
    -Wall
    -Wextra
    -Wpedantic

    -Wfloat-equal
    -Werror

    -O2
)

target_compile_options (${PROJECT_NAME} PRIVATE ${COMPILER_SETTINGS})

