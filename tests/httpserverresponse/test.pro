TARGET = test
TEMPLATE = app

QT       += network
QT       -= gui

OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = bin

INCLUDEPATH += ../../src
LIBS += -L../../lib -ltufao

SOURCES += src/*.cpp

HEADERS += src/*.h
