#
# Vulkan implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Ivan Ivanov, Vladislav Belov, Frolov Daniil
#

cmake_minimum_required(VERSION 3.12)

project("Vulkan")

find_package(Vulkan REQUIRED FATAL_ERROR) # error
# find_package(SFML REQUIRED network audio) # error
find_package(glfw REQUIRED FATAL_ERROR) # error

# Add source to this project's executable.
# add_executable (MyProject "MyProject.cpp")



set(EXEC_FILE "vulkan")


set(SOURCES

    main.cpp

)

target_include_directories (Vulkan 
    PUBLIC ${GLFW_INCLUDE_DIRS}
    # PUBLIC ${SFML_INCLUDE_DIR}
    # PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries (Vulkan glfw)
# target_link_libraries (EXEC_FILE ${VULKAN_LIB_LIST})
# target_link_libraries (EXEC_FILE ${SFML_LIBRARIES})


#executable files
add_executable(${EXEC_FILE} ${SOURCES})

target_include_directories(${EXEC_FILE} PRIVATE )

# target_link_libraries(${TEST_FILE} PRIVATE "${GTEST_LIBRARIES}" "pthread")
# target_include_directories(${TEST_FILE} PRIVATE )

#compiler settings

set (COMPILER_SETTINGS -Wall
-Wextra
-Wpedantic

-Wsign-promo
-Wsign-conversion
-Wdouble-promotion

-Wfloat-equal
-Wbool-compare

-Wdiv-by-zero
-std=c++17 # G++ version in CMAKE (Special command)
-O2 
# For Vulkan
-lglfw 
-lvulkan 
-ldl 
-lpthread 
-lX11 
-lXrandr 

)
target_compile_options( ${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS} )
