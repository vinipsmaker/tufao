# Project Tufão
project(tufao)
cmake_minimum_required(VERSION 2.8)
find_package(Qt4 4.6 REQUIRED QtCore QtNetwork)

# Configure options
set(BUFFER_SIZE 128 CACHE STRING
    "The default buffer size (in bytes) used by Tufão")
add_definitions(-DBUFFER_SIZE=${BUFFER_SIZE})

# Install info
set(TUFAO_VERSION_MAJOR 0)
set(TUFAO_VERSION_MINOR 3)
set(TUFAO_VERSION_PATCH 4)
set(TUFAO_VERSION ${TUFAO_VERSION_MAJOR}.${TUFAO_VERSION_MINOR}.${TUFAO_VERSION_PATCH})
set(includedir include/tufao-${TUFAO_VERSION_MAJOR})
set(libdir lib)

# Targets
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(pkg)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# CPack installer
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "An asynchronous web framework for C++ built on top of Qt")
set(CPACK_PACKAGE_VERSION_MAJOR "${TUFAO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TUFAO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${TUFAO_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER")
include(CPack)
