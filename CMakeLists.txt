cmake_minimum_required(VERSION 3.10)
project(AsmValidator)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(AsmValidator MODULE
  src/InlineAsmValidator.cpp
)

target_link_libraries(AsmValidator PRIVATE
  clang-cpp
  LLVM
)

set_target_properties(AsmValidator PROPERTIES 
    COMPILE_FLAGS "-fPIC"
    PREFIX ""
)
