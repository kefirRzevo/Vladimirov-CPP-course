cmake_minimum_required(VERSION 3.16)

project(paracl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)
set(INTERPETER "${PROJECT_NAME}")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(GTest REQUIRED)

set(CMAKE_BUILD_TYPE Debug)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(COMPILER_SETTINGS -g -O0 -Wall -Wextra -Wpedantic)  
else ()
    set(COMPILER_SETTINGS -O2 -Wall -Wextra -Wpedantic)
endif ()
add_compile_options(${COMPILER_SETTINGS})
include_directories(include ${Boost_INCLUDE_DIRS})

flex_target(
  scanner
  src/frontend/Lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc
)
bison_target(
  parser
  src/frontend/Parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMPILE_FLAGS "-Wcounterexamples --defines=${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh"
)

set(INTERPETER_SOURCES
  src/frontend/AST.cpp
  ParaCL.cpp
  src/backend/Instructions.cpp
)
add_executable(
  ${INTERPETER}
  ${BISON_parser_OUTPUTS}
  ${FLEX_scanner_OUTPUTS}
  ${INTERPETER_SOURCES}
)
#include_directories(${BOOST_INCLUDE_DIRS})
target_include_directories(
  ${INTERPETER} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${INTERPETER} ${Boost_PROGRAM_OPTIONS_LIBRARY})

#enable_testing()
#set(TARGET vm_test)
#set(TEST_SOURCES
#  src/backend/Instructions.cpp
#  test/VMTest.cpp
#)
#add_executable(${TARGET} ${TEST_SOURCES})
#include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(${TARGET} ${GTEST_LIBRARIES})
