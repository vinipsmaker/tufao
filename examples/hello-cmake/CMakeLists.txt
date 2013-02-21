project(test)

cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.7 REQUIRED QtCore QtNetwork)
include(${QT_USE_FILE})

find_package(Tufao0 REQUIRED)
include_directories(${TUFAO_INCLUDE_DIR})

qt4_wrap_cpp(moc mainhandler.h)
add_executable(test main.cpp mainhandler.cpp ${moc})
target_link_libraries(test ${QT_LIBRARIES} ${TUFAO_LIBRARIES})
