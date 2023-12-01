cmake_minimum_required(VERSION 3.16)

project(paracl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(INTERPETER "${PROJECT_NAME}")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(CMAKE_BUILD_TYPE Debug)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(COMPILER_SETTINGS -g -Wall -Wextra -Wpedantic)  
else ()
    set(COMPILER_SETTINGS -O2 -Wall -Wextra -Wpedantic)
endif ()
add_compile_options(${COMPILER_SETTINGS})

include_directories(include)

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

add_flex_bison_dependency(scanner parser)

add_executable(
  ${INTERPETER}
  ${BISON_parser_OUTPUTS}
  ${FLEX_scanner_OUTPUTS}
  src/frontend/AST.cpp ParaCL.cpp
)

target_compile_features(${INTERPETER} PRIVATE cxx_std_20)
target_include_directories(${INTERPETER} PRIVATE includes ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${INTERPETER} PRIVATE ${Boost_LIBRARIES} ${llvm_libs})
