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
# add_executable( ${EXEC_NAME} )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/glfw)
include_directories(3rdparty/glfw/include)

add_subdirectory(3rdparty/glm)
include_directories(3rdparty/glm)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory (vulkan/shaders)
include_directories (${CMAKE_CURRENT_DIR}/vulkan/shaders)

add_executable (${EXEC_NAME})

add_dependencies(${EXEC_NAME} Shaders)

target_include_directories( ${EXEC_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)



# find_package(Vulkan REQUIRED)
 target_include_directories(${EXEC_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
 target_link_libraries(${EXEC_NAME} Vulkan::Vulkan)

# add_custom_command(TARGET ${EXEC_NAME}
	#POST_BUILD
    # COMMAND ${CMAKE_SOURCE_DIR}/vulkan/shaders/compile.sh
    #)

# find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${EXEC_NAME} glfw)

target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/main.cpp" )

foreach( FILE IN LISTS SOURCES)
    target_sources( ${EXEC_NAME} PRIVATE "${EXECSOURCES}/${FILE}" )
endforeach()

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set (COMPILER_SETTINGS -O0 -g -D DEBUG)
    
else ()
    set (COMPILER_SETTINGS -O3 )
    cmake_print_variables(CMAKE_BUILD_TYPE)

endif ()

target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS} )
