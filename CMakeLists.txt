cmake_minimum_required(VERSION 3.28.3)

project(ass2)

set(CMAKE_C_FLAGS "-Wall -Werror -g")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable( ass2
                main.c
                core/cmdproc.c
)
