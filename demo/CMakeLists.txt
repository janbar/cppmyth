cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(livetvdemo)
include(GNUInstallDirs)

if (MSVC)
  # This option must match the settings used in your program, in particular if you
  # are linking statically
  OPTION( STATIC_CRT "Link the static CRT libraries" OFF )
endif (MSVC)

###############################################################################
# add definitions
if (MSVC)
  # Turn off Microsofts "security" warnings.
  add_definitions( "/W3 /D_USE_32BIT_TIME_T /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /nologo" )
  if (STATIC_CRT)
    set(CMAKE_C_FLAGS_RELEASE "/MT")
    set(CMAKE_C_FLAGS_DEBUG "/MTd")
  endif (STATIC_CRT)
endif (MSVC)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_C_FLAGS "-fPIC")
   set(CMAKE_CXX_FLAGS "-fPIC")
endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

###############################################################################
# configure

include_directories (
  ${CMAKE_BINARY_DIR}/cppmyth/include/.)

###############################################################################
# add sources
file(GLOB SRC_FILES src/*.cpp)

set(livetvdemo_SOURCES
  ${SRC_FILES})

###############################################################################
# add targets
add_executable(livetvdemo ${livetvdemo_SOURCES})
add_dependencies(livetvdemo cppmyth jansson)
if(MSVC)
  target_link_libraries(livetvdemo cppmyth jansson ws2_32.lib)
else(MSVC)
  find_library(LIBRT rt)
  if(LIBRT)
    target_link_libraries(livetvdemo cppmyth jansson rt pthread)
  else(LIBRT)
    target_link_libraries(livetvdemo cppmyth jansson pthread)
  endif(LIBRT)
endif(MSVC)

###############################################################################
# install targets
install(TARGETS livetvdemo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
