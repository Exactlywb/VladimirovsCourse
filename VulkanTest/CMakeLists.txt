#
# Vulkan implementation
# if you want see some more you may check github.com/Exactlywb/VladimirovsCourse
# (c) Ivan Ivanov, Vladislav Belov, Frolov Daniil
#

cmake_minimum_required(VERSION 3.12)
set (ProjectName "VulkanTest")
project(${ProjectName})

find_package(Vulkan)

set(EXEC_FILE "vulkan")

set(SOURCES

    main.cpp

)


#  Библиотеки, которые нужны для сборки главной цели

add_executable(${EXEC_FILE} ${SOURCES})

target_link_libraries(${EXEC_FILE}
glfw
dl 
pthread 
X11 
Xrandr
)

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
# -lglfw
# -lvulkan 
# -ldl 
# -lpthread 
# -lX11 
# -lXrandr 

)
target_compile_options( ${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS} )