cmake_minimum_required (VERSION 3.16)
project (triangles)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

#main
set (SOURCES Triangles.cpp)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Werror -Wdollar-in-identifier-extension")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Werror -Wdollar-in-identifier-extension")
set(CMAKE_CXX_FLAGS_INIT "-Werror -Wdollar-in-identifier-extension")
set(CMAKE_CXX_FLAGS "-Werror -Wdollar-in-identifier-extension")

add_executable (${PROJECT_NAME} ${SOURCES})

add_compile_options (-Werror -Wall -Wextra -Wpedantic)

#tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TARGET triangles_test)
set (TEST_SOURCES test/TrianglesTest.cpp)

add_executable(${TARGET} ${TEST_SOURCES})

target_link_libraries (${TARGET} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})