cmake_minimum_required (VERSION 2.8.12...3.20)

project (cppmythroot)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_IGNORE_PATH "/opt/local/bin;/opt/local/lib;/opt/local/include")

include(DownloadProject)

# Provides dependency zlib
find_package (ZLIB QUIET)
if(NOT ZLIB_FOUND)
    download_project(PROJ zlib
        URL "${CMAKE_CURRENT_SOURCE_DIR}/zlib-1.3.1.tar.gz"
        URL_HASH MD5=9855b6d802d7fe5b7bd5b196a2271655
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        UPDATE_DISCONNECTED ON)
    set(BUILD_SHARED_LIBS OFF)
    set(SKIP_INSTALL_ALL ON)
    add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/zlib-src
        ${CMAKE_CURRENT_BINARY_DIR}/zlib-build
        EXCLUDE_FROM_ALL
    )
    set(ZLIB_LIBRARIES zlibstatic)
    set(ZLIB_INCLUDE_DIRS "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
    set(ZLIB_FOUND ON)
    message(STATUS "Build ZLIB: ${zlib_BINARY_DIR}")
endif()

add_subdirectory (${CMAKE_SOURCE_DIR}/cppmyth)
add_subdirectory (${CMAKE_SOURCE_DIR}/demo)

include(CTest)
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)

###############################################################################
# install targets
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
