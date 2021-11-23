cmake_minimum_required(VERSION 3.16)
project(vulktut)


set(CMAKE_CXX_STANDARD 20)


set(EXEC_FILE vulktut)

set(HEADERS

    headers/vulkan.hpp

)

set(SOURCES

    sources/main.cpp
    sources/vulkan.cpp

)

add_executable(${EXEC_FILE} ${SOURCES})
target_include_directories(${EXEC_FILE} PRIVATE headers)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

add_custom_command(TARGET vulktut
    POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/vulkan/shaders/compile.sh
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

set (COMPILER_SETTINGS -Wall -Wextra -O2 -g -Wfloat-equal -Wdiv-by-zero)
target_compile_options( ${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS} )
