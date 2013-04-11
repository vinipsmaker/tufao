project(hello-world)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

find_package(Tufao1 REQUIRED)
include_directories(${TUFAO_INCLUDE_DIR})

find_package(CXX11 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")

add_executable(${PROJECT_NAME} main.cpp)
qt5_use_modules(${PROJECT_NAME} Core Network)
target_link_libraries(${PROJECT_NAME} ${TUFAO_LIBRARIES})
