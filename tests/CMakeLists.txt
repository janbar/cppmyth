cmake_minimum_required (VERSION 3.8.2...3.20)

project (cppmythtest C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###############################################################################
# setup
if (MSVC)
  add_definitions ("/D_CRT_SECURE_NO_WARNINGS")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /RTC1")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /EHsc /nologo")
  set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /O2 /EHsc /nologo")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /EHsc /nologo")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /RTC1 /EHsc /nologo")
endif ()

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD 11)

###############################################################################
# configure
include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/cppmyth/include
  ${CMAKE_SOURCE_DIR}/cppmyth/src)

###############################################################################
# add targets

add_executable (wgettest wgettest.cpp)
target_link_libraries (wgettest cppmyth)

add_executable (cppmythtest cppmythtest.cpp)
target_link_libraries (cppmythtest cppmyth)

###############################################################################
# add unit tests
include(UnitTestProject)

add_library(runner STATIC runner.cpp)

unittest_project(NAME test_builtins SOURCES test_builtins.cpp TARGET runner cppmyth)
unittest_project(NAME test_compressor SOURCES test_compressor.cpp TARGET runner cppmyth)
unittest_project(NAME test_intrinsic SOURCES test_intrinsic.cpp TARGET runner cppmyth)
