cmake_minimum_required (VERSION 2.8.9 FATAL_ERROR)
cmake_policy (VERSION 2.8)

project (cppmythtest)

###############################################################################
# add definitions
if (MSVC)
  add_definitions ("/D_USE_32BIT_TIME_T /D_CRT_SECURE_NO_WARNINGS")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /W3")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W3")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /Od /RTC1 /EHsc /nologo")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /Od /RTC1 /EHsc /nologo")
endif ()

if (NOT MSVC)
  if (COMPILER_SUPPORTS_CXX11)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif ()
endif ()

###############################################################################
# configure
include_directories (
  ${CMAKE_BINARY_DIR}/cppmyth/include/.)

###############################################################################
# add targets
add_executable (cppmythtest src/test.cpp)
add_dependencies (cppmythtest cppmyth)
target_link_libraries (cppmythtest cppmyth)

add_executable (intrinsictest src/intrinsictest.cpp)
add_dependencies (intrinsictest cppmyth)
target_link_libraries (intrinsictest cppmyth)
