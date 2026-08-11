cmake_minimum_required (VERSION 2.8.12...3.20)

project (cppmythtest C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###############################################################################
# add definitions
if (MSVC)
  add_definitions ("/D_CRT_SECURE_NO_WARNINGS")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /W3")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W3")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /Od /RTC1 /EHsc /nologo")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /Od /RTC1 /EHsc /nologo")
endif ()

if (NOT MSVC)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message(STATUS "Using standard c++11")
  endif()
endif()

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
