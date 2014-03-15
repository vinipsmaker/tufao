TARGET = threads-example
TEMPLATE = app

CONFIG += C++11 TUFAO1
QT -= gui

SOURCES += main.cpp \
    tcpserver.cpp \
    worker.cpp

HEADERS += tcpserver.h \
    worker.h
