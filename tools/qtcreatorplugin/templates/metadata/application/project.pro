TARGET = %1
TEMPLATE = app

CONFIG += TUFAO
QT -= gui

SOURCES += main.cpp \
    notfound.cpp \
    pluginreloader.cpp

HEADERS += \
    notfound.h \
    pluginreloader.h

RESOURCES += \
    static.qrc
