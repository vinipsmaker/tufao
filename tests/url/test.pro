TARGET = test
TEMPLATE = app

CONFIG += TUFAO
QT     -= gui

OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = bin

SOURCES += src/*.cpp

HEADERS += src/*.h
