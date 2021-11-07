cmake_minimum_required(VERSION 3.16)

project (Matrix)               # PROJECT TITLE

include(CMakePrintHelpers)

set(EXEC_NAME "matrix" )
add_executable( ${EXEC_NAME} )

target_include_directories( ${EXEC_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

set(EXECSOURCES "${CMAKE_SOURCE_DIR}/sources" )

set(SOURCES
    "matrix.cpp"
    "main.cpp"
)

target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/sources/main.cpp" )

foreach( FILE IN LISTS SOURCES)
    target_sources( ${EXEC_NAME} PRIVATE "${EXECSOURCES}/${FILE}" )
endforeach()

set (COMPILER_SETTINGS

    -O2
    -Wall
    -Wextra
    -Wpedantic

)

target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS} )

# target_compile_options( ${EXEC_NAME} PRIVATE ${COMPILER_SETTINGS})