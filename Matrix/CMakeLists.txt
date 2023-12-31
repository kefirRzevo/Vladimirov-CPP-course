cmake_minimum_required (VERSION 3.16)
project (matrix)

set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

#main
set (SOURCES Matrix.cpp)

add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories (${PROJECT_NAME} PRIVATE includes)

add_compile_options (-Werror -Wall -Wextra -Wpedantic)

#tests
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TARGET matrix_test)
set (TEST_SOURCES test/MatrixTest.cpp)

add_executable(${TARGET} ${TEST_SOURCES})

target_include_directories (${TARGET} PRIVATE includes)
target_link_libraries (${TARGET} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
