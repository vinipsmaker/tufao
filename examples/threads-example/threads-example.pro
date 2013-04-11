TARGET = threads-example
TEMPLATE = app

CONFIG += C++11 TUFAO1
QT -= gui

SOURCES += main.cpp \
    tcpserver.cpp \
    handlerfactory.cpp \
    worker.cpp

HEADERS += tcpserver.h \
    handlerfactory.h \
    worker.h
