CMAKE_MINIMUM_REQUIRED(VERSION 2.8.10)

#######################################################################################################################
# Project name
project(sample-plugin)
set(sample-plugin_MAJOR "0")
set(sample-plugin_MINOR "1")
set(sample-plugin_PATCH "0")
set(VERSION "${sample-plugin_MAJOR}.${sample-plugin_MINOR}.${sample-plugin_PATCH}")
set(PACKAGE sample-plugin)

set(PACKAGE_VERSION "${VERSION}")
add_definitions(-DPACKAGE_VERSION="${PACKAGE_VERSION}")

########### Main global variables ###########
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug GProf Valgrind Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(BUILDTYPE ${CMAKE_BUILD_TYPE})
string(TOUPPER ${BUILDTYPE} BUILDTYPE)
add_definitions(-D${BUILDTYPE})

# We have some custom .cmake scripts not in the official distribution.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

# Add valgrind build options if necessary
IF(${CMAKE_BUILD_TYPE} MATCHES "Valgrind")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
ENDIF()

#Where plugins are located
set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}")
add_definitions(-DINSTALL_PLUGIN_DIR="${CMAKE_INSTALL_PREFIX}/plugins")

#######################################################################################################################
# 3rd Party Dependency Stuff
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

find_package(Tufao1 REQUIRED)

find_package(CXX11 REQUIRED)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


#######################################################################################################################
# Finish configuring compiler / linker settings & flags
include_directories(${TUFAO_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")


#######################################################################################################################
# Add subdirectories
add_subdirectory(src)
add_subdirectory(plugins)
# add_subdirectory(test)

#######################################################################################################################
# enable testing - must be in the top level CMakeLists.txt file
# enable_testing()

