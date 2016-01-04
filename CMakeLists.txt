# Project Tufão
project(tufao)

# Metadata
set(TUFAO_VERSION_MAJOR 1)
set(TUFAO_VERSION_MINOR 3)
set(TUFAO_VERSION_PATCH 5)
set(TUFAO_VERSION ${TUFAO_VERSION_MAJOR}.${TUFAO_VERSION_MINOR}.${TUFAO_VERSION_PATCH})
set(TUFAO_BRIEF "An asynchronous web framework for C++ built on top of Qt")

# Dependencies
cmake_minimum_required(VERSION 2.8.11)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

# CMake since 3.1 has ability to use particular C/C++ standarts.
# For versions before 3.1 use FindCXX11 module as fallback.
if("${CMAKE_VERSION}" VERSION_LESS "3.1")
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
    find_package(CXX11 REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
endif()

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

# Debug version of library should have 'd' postfix on Windows
# and '_debug' on Mac OS X
if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "d")
elseif(APPLE)
    set(CMAKE_DEBUG_POSTFIX "_debug")
endif()

# Configure options
option(GENERATE_DOC
    "Use Doxygen to generate the project documentation" OFF
)

option(ENABLE_TESTS
    "Generate and run tests" OFF
)

set(BUFFER_SIZE 128 CACHE STRING
    "The default buffer size (in bytes) used by Tufão")
add_definitions(-DBUFFER_SIZE=${BUFFER_SIZE})

# Build info
set(TUFAO_LIBRARY "tufao${TUFAO_VERSION_MAJOR}")

# Install info
set(includedir include/tufao-${TUFAO_VERSION_MAJOR})
set(libdir "lib${LIB_SUFFIX}")

# Targets
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(pkg)
add_subdirectory(doc)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
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
    mark_as_advanced(BUILD_TESTING)
endif()

# cmake-modules
set(TUFAO_CONFIG_INSTALL_DIR "${libdir}/cmake/Tufao${TUFAO_VERSION_MAJOR}")
include(CMakePackageConfigHelpers)
configure_package_config_file(
  TufaoConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/Tufao${TUFAO_VERSION_MAJOR}Config.cmake"
  INSTALL_DESTINATION ${TUFAO_CONFIG_INSTALL_DIR}
  PATH_VARS libdir
            includedir)

write_basic_config_version_file(Tufao${TUFAO_VERSION_MAJOR}ConfigVersion.cmake VERSION ${TUFAO_VERSION} COMPATIBILITY ExactVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Tufao${TUFAO_VERSION_MAJOR}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/Tufao${TUFAO_VERSION_MAJOR}ConfigVersion.cmake
        DESTINATION ${TUFAO_CONFIG_INSTALL_DIR})

install(EXPORT "${TUFAO_LIBRARY}-export" DESTINATION ${TUFAO_CONFIG_INSTALL_DIR} FILE Tufao${TUFAO_VERSION_MAJOR}Targets.cmake)
