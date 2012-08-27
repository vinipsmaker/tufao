TARGET = sessionusage
TEMPLATE = app

CONFIG += TUFAO0
QT -= gui

SOURCES += main.cpp \
    notfound.cpp \
    readhandler.cpp \
    sethandler.cpp \
    unsethandler.cpp \
    roothandler.cpp

HEADERS += \
    notfound.h \
    readhandler.h \
    sethandler.h \
    unsethandler.h \
    roothandler.h

RESOURCES += \
    static.qrc
