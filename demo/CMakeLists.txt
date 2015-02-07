cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
cmake_policy (VERSION 2.6)

project (livetvdemo)
#include (GNUInstallDirs)

if (MSVC)
  # This option must match the settings used in your program, in particular if you
  # are linking statically
  OPTION (STATIC_CRT "Link the static CRT libraries" OFF)
endif ()

###############################################################################
# add definitions
if (MSVC)
  # Turn off Microsofts "security" warnings.
  add_definitions ("/W3 /D_USE_32BIT_TIME_T /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /nologo")
  if (STATIC_CRT)
    set (CMAKE_C_FLAGS_RELEASE "/MT")
    set (CMAKE_C_FLAGS_DEBUG "/MTd")
  endif ()
endif ()

if (NOT WIN32 AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX))
   add_definitions ("-fPIC")
endif ()

###############################################################################
# configure

include_directories (
  ${CMAKE_BINARY_DIR}/cppmyth/include/.)

###############################################################################
# add sources
file (GLOB SRC_FILES src/*.cpp)

set (LIVETVDEMO_SOURCES
  ${SRC_FILES})

###############################################################################
# add targets
add_executable (livetvdemo ${LIVETVDEMO_SOURCES})
add_dependencies (livetvdemo cppmyth jansson)
set (DEPLIBS cppmyth jansson)
if (MSVC)
  list (APPEND DEPLIBS ws2_32)
else ()
  list (APPEND DEPLIBS pthread m)
  find_library (LIBRT rt)
  if (LIBRT)
    list (APPEND DEPLIBS rt)
  endif ()
endif ()
target_link_libraries (livetvdemo ${DEPLIBS})
