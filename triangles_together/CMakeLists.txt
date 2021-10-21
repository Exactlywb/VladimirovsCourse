cmake_minimum_required( VERSION 3.16)

project (Triangles VERSION 1.0.1)               # PROJECT TITLE

set( EXEC_NAME "triangles" )
add_executable( ${EXEC_NAME} )

target_include_directories( ${EXEC_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

set( EXECSOURCES "${CMAKE_SOURCE_DIR}/source" )

set( SOURCES
    "intersect.cpp"
    "common.cpp"
    "octree.cpp"
    "triangle.cpp"
    "vector.cpp"
  )


set (COMPILER_SETTINGS -Wall -Wextra -O2)       # COMPILER SETTINGS

target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/main.cpp" )

foreach( FILE IN LISTS SOURCES)
    target_sources( ${EXEC_NAME} PRIVATE "${EXECSOURCES}/${FILE}" )
endforeach()

set( DEBUG_DEFINE_NAME "DEBUG" )
set( DEBUG_FLAGS -D${DEBUG_DEFINE_NAME}=1 -O0 -g -Wall -Wextra -Wpedantic  )
set( RELEASE_FLAGS -D${DEBUG_DEFINE_NAME}=0 -O2 )

target_compile_options( ${EXEC_NAME} PRIVATE ${DEBUG_FLAGS} )
target_compile_options( ${EXEC_NAME} PRIVATE ${RELEASE_FLAGS} )
target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS})