cmake_minimum_required (VERSION 3.8.2...3.20)

project (livetvdemo)

###############################################################################
# setup
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD 11)

###############################################################################
# configure
include_directories (
  ${CMAKE_BINARY_DIR}/cppmyth/include)

###############################################################################
# add sources
file (GLOB SRC_FILES src/*.cpp)

set (LIVETVDEMO_SOURCES
  ${SRC_FILES})

###############################################################################
# add targets
add_executable (livetvdemo ${LIVETVDEMO_SOURCES})
add_dependencies (livetvdemo cppmyth)
target_link_libraries (livetvdemo cppmyth)
