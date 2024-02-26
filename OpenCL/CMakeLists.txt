cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 20)

project(bsort)

find_package(OpenCL REQUIRED)
include_directories(include)

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set (COMPILER_SETTINGS -g -Wall -Wextra -Wpedantic)
else ()
    set (COMPILER_SETTINGS -g -Wall -Wextra -Wpedantic)
endif ()
add_compile_options(${COMPILER_SETTINGS})

set (SOURCES OpenCL.cpp src/GpuBitonicSort.cpp)

add_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_libs} "-framework OpenCL")

enable_testing()
find_package(GTest REQUIRED)
set (TARGET bsort_test)
set (TEST_SOURCES test/OpenClTest.cpp src/GpuBitonicSort.cpp)
add_executable(${TARGET} ${TEST_SOURCES})
target_include_directories (${TARGET} PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries (${TARGET} ${GTEST_MAIN_LIBRARIES} "-framework OpenCL")
