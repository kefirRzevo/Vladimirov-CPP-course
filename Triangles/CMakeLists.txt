cmake_minimum_required (VERSION 3.16)
project (triangles)

set (CMAKE_CXX_STANDARD 20)

#main
set (SOURCES Triangles.cpp)

add_executable (${PROJECT_NAME} ${SOURCES})

add_compile_options (-Werror -Wall -Wextra -Wpedantic)

#tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TARGET triangles_test)
set (TEST_SOURCES test/TrianglesTest.cpp)

add_executable(${TARGET} ${TEST_SOURCES})

target_link_libraries (${TARGET} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
