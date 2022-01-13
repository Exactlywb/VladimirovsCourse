#
# ParaCL implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Ivan Ivanov, Vladislav Belov, Frolov Daniil
#
cmake_minimum_required (VERSION 3.13.4)
project (ParaCL)

# Here we're trying to find Bison and Flex
find_package (BISON REQUIRED)
find_package (FLEX REQUIRED)

if (NOT DEFINED GRAMMAR)

    set (GRAMMAR "frontend/parser/grammar.y" CACHE STRING "grammar file" FORCE)

endif ()

flex_target (scanner

    frontend/lexer/paraCL.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc

)

bison_target (parser

    ${GRAMMAR}
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
    COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hh"

)

add_flex_bison_dependency (scanner parser)

add_executable (${PROJECT_NAME}

    backend/interpreter/interpreter.cpp
    frontend/driver/driver.cpp
    frontend/AST/ast.cpp
    ${BISON_parser_OUTPUTS}
    ${FLEX_scanner_OUTPUTS}

)

target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/frontend/AST")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/frontend/driver")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/common/InputHandler/")

target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/backend/interpreter/")

