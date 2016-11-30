cmake_minimum_required (VERSION 3.0.2 FATAL_ERROR)
cmake_policy (VERSION 3.0)

add_subdirectory (${CMAKE_SOURCE_DIR}/cppmyth)
add_subdirectory (${CMAKE_SOURCE_DIR}/test)
add_subdirectory (${CMAKE_SOURCE_DIR}/demo)

###############################################################################
# install targets
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
