TARGET = %1
TEMPLATE = lib

QT -= gui
QT += core
CONFIG += plugin TUFAO0

DEFINES += PLUGIN=%1

SOURCES += plugin.cpp \
    requesthandler.cpp

HEADERS += plugin.h \
    requesthandler.h
