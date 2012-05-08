TARGET = ThreadsExample
TEMPLATE = app

CONFIG += TUFAO
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
