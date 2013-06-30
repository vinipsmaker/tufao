# Project Tufão
project(tufao)

# Metadata
set(TUFAO_VERSION_MAJOR 0)
set(TUFAO_VERSION_MINOR 8)
set(TUFAO_VERSION_PATCH 0)
set(TUFAO_VERSION ${TUFAO_VERSION_MAJOR}.${TUFAO_VERSION_MINOR}.${TUFAO_VERSION_PATCH})
set(TUFAO_BRIEF "An asynchronous web framework for C++ built on top of Qt")

# Dependencies
cmake_minimum_required(VERSION 2.8)
if (USE_QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Network REQUIRED)

    get_target_property(QMAKE_EXECUTABLE Qt5::qmake LOCATION)
    function(QUERY_QMAKE VAR RESULT)
        exec_program("${QMAKE_EXECUTABLE}" ARGS "-query ${VAR}" RETURN_VALUE return_code OUTPUT_VARIABLE output)
        if(NOT return_code)
            file(TO_CMAKE_PATH "${output}" output)
            set(${RESULT} ${output} PARENT_SCOPE)
        endif(NOT return_code)
    endfunction(QUERY_QMAKE)

    query_qmake(QT_INSTALL_PLUGINS QT_PLUGINS_DIR)
    query_qmake(QT_HOST_DATA QT_DATA_DIR)

    set(QT_MKSPECS_DIR ${QT_DATA_DIR}/mkspecs)
else (USE_QT5)
    find_package(Qt4 4.7 REQUIRED QtCore QtNetwork)
endif (USE_QT5)

# Configure options
option(GENERATE_DOC
    "Use Doxygen to generate the project documentation" OFF
)

option(ENABLE_TESTS
    "Generate and run tests" OFF
)

option(USE_QT5
    "Use Qt5 to build Tufão" OFF
)

set(BUFFER_SIZE 128 CACHE STRING
    "The default buffer size (in bytes) used by Tufão")
add_definitions(-DBUFFER_SIZE=${BUFFER_SIZE})

# Build info
set(TUFAO_LIBRARY "tufao")
if (USE_QT5)
    set(TUFAO_LIBRARY "${TUFAO_LIBRARY}-qt5")
endif()

# Install info
set(includedir include/tufao-${TUFAO_VERSION_MAJOR})
set(libdir "lib${LIB_SUFFIX}")
if (USE_QT5)
    set(includedir ${includedir}-qt5)
endif()

# Targets
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(pkg)

if (GENERATE_DOC)
    add_subdirectory(doc)
endif()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# CPack installer
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${TUFAO_BRIEF}")
set(CPACK_PACKAGE_VERSION_MAJOR "${TUFAO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TUFAO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${TUFAO_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER")
include(CPack)

# Testing
if(ENABLE_TESTS)
    include(CTest)
endif()
