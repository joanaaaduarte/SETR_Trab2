cmake_minimum_required(VERSION 3.28.3)

project(ass2)

set(CMAKE_C_FLAGS "-Wall -Werror -g")

include_directories(${CMAKE_SOURCE_DIR}/core/include
                    ${CMAKE_SOURCE_DIR}/core/src
                    ${CMAKE_SOURCE_DIR}/core/test
                    )

 add_executable(ass2
     ${CMAKE_SOURCE_DIR}/main.c
     ${CMAKE_SOURCE_DIR}/core/src/cmdproc.c
 )  
                  
add_executable(test_runner
    ${CMAKE_SOURCE_DIR}/core/test/test.c
    ${CMAKE_SOURCE_DIR}/core/src/cmdproc.c
    ${CMAKE_SOURCE_DIR}/core/src/unity.c
)

enable_testing()
add_test(NAME TestRunner COMMAND test_runner)
