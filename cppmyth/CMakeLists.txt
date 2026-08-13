cmake_minimum_required (VERSION 3.8.2...3.20)

project (cppmyth)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_language(CXX)
enable_language(C)

# Options
OPTION (BUILD_SHARED_LIBS "Build shared libraries." OFF)

###############################################################################
# set lib version here
set (PACKAGE_VERSION_MAJOR 3)
set (PACKAGE_VERSION_MINOR 0)
set (PACKAGE_VERSION_PATCH 0)

set (PACKAGE_VERSION ${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH})
set (PACKAGE_LIB_SOVERSION ${PACKAGE_VERSION_MAJOR})

###############################################################################
# setup
if(MSVC)
  # This option must match the settings used in your program, in particular
  # if you are linking statically
  option(MSVC_DYNAMIC_RUNTIME "MSVC: using dynamic runtime." OFF)
  include (MSVCRuntime)
  configure_msvc_runtime()
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
include (CheckFunctionExists)
include (CheckFunctionKeywords)
find_package (Threads REQUIRED)

check_function_exists (timegm CHK_TIMEGM)
if (CHK_TIMEGM)
    set (HAVE_TIMEGM 1)
else ()
    set (HAVE_TIMEGM 0)
endif ()

check_function_exists (localtime_r CHK_LOCALTIME_R)
if (CHK_LOCALTIME_R)
    set (HAVE_LOCALTIME_R 1)
else ()
    set (HAVE_LOCALTIME_R 0)
endif ()

check_function_exists (gmtime_r CHK_GMTIME_R)
if (CHK_GMTIME_R)
    set (HAVE_GMTIME_R 1)
else ()
    set (HAVE_GMTIME_R 0)
endif ()

if (NOT ZLIB_FOUND)
  find_package (ZLIB REQUIRED)
endif ()
if (ZLIB_FOUND)
    include_directories (${ZLIB_INCLUDE_DIRS})
    set (HAVE_ZLIB 1)
else ()
    set (HAVE_ZLIB 0)
endif ()

if (NOT OPENSSL_FOUND)
  find_package(OpenSSL QUIET)
endif ()
if (OPENSSL_FOUND)
  include_directories (${OPENSSL_INCLUDE_DIR})
  set(HAVE_OPENSSL 1)
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL ssl library: ${OPENSSL_SSL_LIBRARY}")
  message(STATUS "OpenSSL crypto library: ${OPENSSL_CRYPTO_LIBRARY}")
else ()
  set(HAVE_OPENSSL 0)
endif ()

# Check what the inline keyword is.
check_function_keywords ("inline")
check_function_keywords ("__inline")
check_function_keywords ("__inline__")
if (HAVE_INLINE)
   set (CC_INLINE inline)
elseif (HAVE___INLINE)
   set (CC_INLINE __inline)
elseif (HAVE___INLINE__)
   set (CC_INLINE __inline__)
else ()
   # no inline on this platform
   set (CC_INLINE)
endif ()

# configure local config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/src/local_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/include/local_config.h)

# Support building as a subproject of a larger cmake project
file(GLOB PUBLIC_HDR_FILES
  src/mythlocked.h
  src/mythsharedptr.h
  src/mythtypes.h
  src/mythcontrol.h
  src/mytheventhandler.h
  src/mythfileplayback.h
  src/mythlivetvplayback.h
  src/mythrecordingplayback.h
  src/mythwsapi.h
  src/mythdebug.h
  src/mythstream.h
  src/mythwsstream.h
  src/proto/mythprotobase.h
  src/proto/mythprotomonitor.h
  src/proto/mythprotorecorder.h
  src/proto/mythprotoevent.h
  src/proto/mythprotoplayback.h
  src/proto/mythprototransfer.h)

foreach(file ${PUBLIC_HDR_FILES})
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "" RPATH "${file}")
  configure_file(${file} ./include/${RPATH} COPYONLY)
endforeach()

include_directories (${CMAKE_CURRENT_BINARY_DIR}/include/.)

###############################################################################
# add sources
file (GLOB SRC_FILES
  src/private/mythdto/*.cpp
  src/private/*.c src/private/*.cpp
  src/proto/*.cpp
  src/*.cpp)

file (GLOB OS_SRC_FILES
  src/private/os/threads/threadpool.cpp
  src/private/os/threads/latch.cpp)

if (MSVC)
  list (APPEND OS_SRC_FILES
    src/private/os/windows/winpthreads.c)
endif ()

set (CPPMYTH_SOURCES
  ${SRC_FILES} ${OS_SRC_FILES})

file (GLOB HDR_FILES
  src/private/mythdto/*.h
  src/private/*.h
  src/proto/*.h
  src/*.h)

file (GLOB OS_HDR_FILES
  src/private/os/threads/*.h)

set (CPPMYTH_HEADERS
  ${HDR_FILES} ${OS_HDR_FILES})

###############################################################################
# add targets
set (cppmyth_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
if (ZLIB_FOUND)
  list (APPEND cppmyth_LIBRARIES ${ZLIB_LIBRARIES})
endif ()
if (OPENSSL_FOUND)
  list (APPEND cppmyth_LIBRARIES ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
endif ()
if (MSVC)
  list (APPEND cppmyth_LIBRARIES ws2_32)
else ()
  list (APPEND cppmyth_LIBRARIES m)
  find_library (LIBRT rt)
  if (LIBRT)
    list (APPEND cppmyth_LIBRARIES rt)
  endif ()
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
  find_library (SOCKET socket REQUIRED)
  list (APPEND cppmyth_LIBRARIES socket)
endif ()

if (BUILD_SHARED_LIBS)
  add_library (cppmyth SHARED ${CPPMYTH_SOURCES})
else ()
  add_library (cppmyth STATIC ${CPPMYTH_SOURCES})
endif ()
target_link_libraries (cppmyth ${cppmyth_LIBRARIES})
set_target_properties (cppmyth PROPERTIES
    VERSION "${PACKAGE_VERSION}"
    SOVERSION "${PACKAGE_LIB_SOVERSION}")

###############################################################################
# install targets
# Create pkg-conf file.
# (We use the same files as ./configure does, so we
#  have to defined the same variables used there).
if (NOT MSVC)
  if (NOT DEFINED CMAKE_INSTALL_BINDIR)
    set (CMAKE_INSTALL_BINDIR bin)
  endif ()
  if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set (CMAKE_INSTALL_LIBDIR lib)
  endif ()
  if (NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set (CMAKE_INSTALL_INCLUDEDIR include)
  endif ()
  set (prefix      ${CMAKE_INSTALL_PREFIX})
  set (exec_prefix ${CMAKE_INSTALL_PREFIX})
  set (libdir      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
  configure_file (cppmyth.pc.in cppmyth.pc @ONLY)

  install (TARGETS cppmyth
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

  install (FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cppmyth.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif ()

install (FILES
  ${CMAKE_CURRENT_BINARY_DIR}/include/local_config.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythlocked.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythsharedptr.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythtypes.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythcontrol.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mytheventhandler.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythfileplayback.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythlivetvplayback.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythrecordingplayback.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythwsapi.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythdebug.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythstream.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/mythwsstream.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppmyth/)

install (FILES
  ${CMAKE_CURRENT_BINARY_DIR}/include/proto/mythprotobase.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/proto/mythprotomonitor.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/proto/mythprotorecorder.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/proto/mythprotoevent.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/proto/mythprotoplayback.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/proto/mythprototransfer.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppmyth/proto/)
