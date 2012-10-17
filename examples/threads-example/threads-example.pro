TARGET = threads-example
TEMPLATE = app

CONFIG += TUFAO0
QT -= gui

SOURCES += main.cpp \
    mainhandler.cpp \
    tcpserver.cpp \
    thread.cpp \
    handlerfactory.cpp

HEADERS += mainhandler.h \
    tcpserver.h \
    thread.h \
    handlerfactory.h
