cmake_minimum_required (VERSION 3.13.4)
project (matrix)

add_subdirectory(3rdparty/googletest)
find_package(GTest REQUIRED)

set (TEST_EXEC "unitTests")

add_executable (${PROJECT_NAME} sources/main.cpp)
add_executable (${TEST_EXEC} sources/unitTests.cpp)

target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories (${TEST_EXEC} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${TEST_EXEC} PRIVATE "${GTEST_LIBRARIES}" "pthread")
target_include_directories(${TEST_EXEC} PRIVATE headers "${GTEST_INCLUDE_DIRS}")
