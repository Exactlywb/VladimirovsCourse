cmake_minimum_required( VERSION 3.16)

project (Triangles VERSION 1.0.1)               # PROJECT TITLE

include(CMakePrintHelpers)

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


# set (COMPILER_SETTINGS -Wall -Wextra -O2)       # COMPILER SETTINGS

target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/main.cpp" )

foreach( FILE IN LISTS SOURCES)
    target_sources( ${EXEC_NAME} PRIVATE "${EXECSOURCES}/${FILE}" )
endforeach()

    if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set (COMPILER_SETTINGS -Wall -Wextra -O2 -g -D DEBUG)
        
    else ()
        set (COMPILER_SETTINGS -Wall -Wextra -O2 -Wfloat-equal -Wdiv-by-zero)
        cmake_print_variables(CMAKE_BUILD_TYPE)

    endif ()

    target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS} )

# target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS})
