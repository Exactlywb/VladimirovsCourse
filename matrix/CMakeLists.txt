
cmake_minimum_required( VERSION 3.16)

project (Matrix)

set( EXEC_NAME "matrix" )
add_executable( ${EXEC_NAME} )

target_include_directories( ${EXEC_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

set( EXECSOURCES "${CMAKE_SOURCE_DIR}/sources" )

set( SOURCES
    "main.cpp"
    "matrix.cpp"
)

foreach( FILE IN LISTS SOURCES)
    target_sources( ${EXEC_NAME} PRIVATE "${EXECSOURCES}/${FILE}" )
endforeach()


