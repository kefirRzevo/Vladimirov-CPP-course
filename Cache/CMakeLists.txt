cmake_minimum_required (VERSION 3.16)
project (cache)

set (CMAKE_CXX_STANDARD 17)

#main
set (SOURCES Cache.cpp)

add_executable (${PROJECT_NAME} ${SOURCES})

add_compile_options (-Werror -Wall -Wextra -Wpedantic)

#tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TARGET lfu_test)
set (TEST_SOURCES test/LFUCacheTest.cpp)

add_executable(${TARGET} ${TEST_SOURCES})

target_link_libraries (${TARGET} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

set (TARGET ideal_test)
set (TEST_SOURCES test/IdealCacheTest.cpp)

add_executable(${TARGET} ${TEST_SOURCES})

target_link_libraries (${TARGET} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})