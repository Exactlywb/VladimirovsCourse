cmake_minimum_required(VERSION 3.16)

project (Triangles VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 20)
set(EXECSOURCES "${CMAKE_SOURCE_DIR}/source" )

set(SOURCES
    "intersect.cpp"
    "octree.cpp"
    "triangle.cpp"
    "vector.cpp"
    "vulkan.cpp"
)
  
include(CMakePrintHelpers)

set( EXEC_NAME "triangles" )
add_executable( ${EXEC_NAME} )

target_include_directories( ${EXEC_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

find_package(Vulkan REQUIRED)
target_include_directories(${EXEC_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${EXEC_NAME} Vulkan::Vulkan)
   
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${EXEC_NAME} glfw)

find_package(glm CONFIG REQUIRED)
# target_link_libraries(${EXEC_NAME} glfw)

add_custom_command(TARGET ${EXEC_NAME}
    POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/vulkan/shaders/compile.sh
)


target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/main.cpp" )

foreach( FILE IN LISTS SOURCES)
    target_sources( ${EXEC_NAME} PRIVATE "${EXECSOURCES}/${FILE}" )
endforeach()

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set (COMPILER_SETTINGS -Wall -Wextra -O0 -g -D DEBUG)
    
else ()
    set (COMPILER_SETTINGS -Wall -Wextra -O3 -Wfloat-equal -Wdiv-by-zero)
    cmake_print_variables(CMAKE_BUILD_TYPE)

endif ()

target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS} )
