# Project Tufão
cmake_minimum_required(VERSION 2.8)
find_package(Qt4 4.6 REQUIRED QtCore QtNetwork)

# Install info
set(version_major 0)
set(version ${version_major}.3.4)
set(includedir include/tufao${version_major})
set(libdir lib)

# Targets
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(pkg)
