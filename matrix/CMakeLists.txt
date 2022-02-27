cmake_minimum_required (VERSION 3.13.4)
project (matrix)

add_executable (${PROJECT_NAME} sources/main.cpp)
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

