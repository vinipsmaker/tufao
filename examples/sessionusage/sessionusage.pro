TARGET = sessionusage
TEMPLATE = app

CONFIG += C++11 TUFAO1
QT -= gui

SOURCES += main.cpp \
    readhandler.cpp \
    sethandler.cpp \
    unsethandler.cpp

HEADERS += \
    readhandler.h \
    sethandler.h \
    unsethandler.h

RESOURCES += \
    static.qrc
