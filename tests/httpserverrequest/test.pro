TARGET = test
TEMPLATE = app

QT       += network
QT       -= gui

OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = bin

CONFIG += DEBUG
INCLUDEPATH += ../../src
LIBS += -L../../lib -ltufao

SOURCES += src/main.cpp \
    src/stringlistsocket.cpp \
    src/test1.cpp \
    src/testsguide.cpp \
    src/abstracttest.cpp

HEADERS += src/stringlistsocket.h \
    src/test1.h \
    src/testsguide.h \
    src/abstracttest.h






