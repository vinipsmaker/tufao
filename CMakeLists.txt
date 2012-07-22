# Project Tufão
cmake_minimum_required(VERSION 2.8)
find_package(Qt4 4.6 REQUIRED QtCore QtNetwork)

# Install info
set(TUFAO_VERSION_MAJOR 0)
set(TUFAO_VERSION_MINOR 3)
set(TUFAO_VERSION_PATCH 4)
set(TUFAO_VERSION ${TUFAO_VERSION_MAJOR}.${TUFAO_VERSION_MINOR}.${TUFAO_VERSION_PATCH})
set(includedir include/tufao${TUFAO_VERSION_MAJOR})
set(libdir lib)

# Targets
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(pkg)
