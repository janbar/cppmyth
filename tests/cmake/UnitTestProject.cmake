macro(unittest_project)
    set(_name)
    set(_sources)
    set(_targets)
    set(_includes
        include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    set(_command)
    set(_skiptest OFF)
    set(cmd "_name")
    foreach(arg ${ARGN})
        if(arg STREQUAL "NAME")
            set(cmd "_name")
        elseif(arg STREQUAL "SOURCES")
            set(cmd "_sources")
        elseif(arg STREQUAL "TARGET")
            set(cmd "_targets")
        elseif(arg STREQUAL "INCLUDES")
            set(cmd "_includes")
        elseif(arg STREQUAL "COMMAND")
            set(cmd "_command")
        elseif(arg STREQUAL "SKIPTEST")
            set(_skiptest ON)
        else()
            if("X${cmd}" STREQUAL "X_name")
                set(_name ${arg})
            else()
                list(APPEND ${cmd} ${arg})
            endif()
        endif()
    endforeach()
    add_executable(${_name} ${_sources})
    target_link_libraries(${_name} ${_targets})
    target_include_directories(${_name} PRIVATE ${_includes})
    if(NOT _skiptest)
        add_test(NAME ${_name} COMMAND ${_name} ${_command} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endmacro(unittest_project)
