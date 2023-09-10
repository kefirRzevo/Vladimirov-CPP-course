cmake_minimum_required (VERSION 3.16)
project (triangles)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

#main
set (SOURCES Triangles.cpp)

add_executable (${PROJECT_NAME} ${SOURCES})

add_compile_options (-Werror -Wall -Wextra -Wpedantic)

#tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TARGET geometry_test)
set (TEST_SOURCES test/GeometryTest.cpp)

set (TARGET1 problem_test)
set (TEST_SOURCES1 test/ProblemTest.cpp)

add_executable(${TARGET} ${TEST_SOURCES})
add_executable(${TARGET1} ${TEST_SOURCES1})

target_link_libraries (${TARGET} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
target_link_libraries (${TARGET1} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
