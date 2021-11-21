cmake_minimum_required(VERSION 3.16)
project(vulktut)


set(CMAKE_CXX_STANDARD 20)


add_executable(vulktut main.cpp)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

add_custom_command(TARGET vulktut
    POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/vulkan/shaders/compile.sh
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

