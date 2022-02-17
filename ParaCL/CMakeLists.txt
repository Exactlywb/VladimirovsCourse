#
# ParaCL implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Ivan Ivanov, Vladislav Belov, Frolov Daniil
#
cmake_minimum_required (VERSION 3.13.4)
project (ParaCL)

include(CMakePrintHelpers)

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

# LLVM including
find_package (LLVM REQUIRED CONFIG)
message (STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message (STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories (${LLVM_INCLUDE_DIRS})
separate_arguments (LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions (${LLVM_DEFINITIONS_LIST})

# Now build our source files
add_executable (${PROJECT_NAME}

    backend/interpreter/interpreter.cpp
    backend/compiler/compiler.cpp
    frontend/driver/driver.cpp
    frontend/customLexer/customLexer.cpp
    frontend/SemanticAnalyzer/semantic.cpp
    ${BISON_parser_OUTPUTS}
    ${FLEX_scanner_OUTPUTS}

)

# Find the llvm libraries
llvm_map_components_to_libnames (llvm_libs support core irreader)

# Link
target_link_libraries (${PROJECT_NAME} ${llvm_libs})

target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#target_include_directories for FRONTEND
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/frontend/AST")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/frontend/driver")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/frontend/customLexer/")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/frontend/SemanticAnalyzer/")

#target_include_directories for COMMON
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/common/")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/common/InputHandler/")

#target_include_directories for BACKEND
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/backend/interpreter/")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/backend/compiler/")


find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/*.[ch]pp
        ${PROJECT_SOURCE_DIR}/*.[ch]
    )

    add_custom_target(
        format
        COMMAND clang-format
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
endif()
