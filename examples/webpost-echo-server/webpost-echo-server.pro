TARGET = webpost-echo-server
TEMPLATE = app

CONFIG += TUFAO0
QT -= gui

SOURCES += main.cpp \
    mainhandler.cpp \
    posthandler.cpp

HEADERS += mainhandler.h \
    posthandler.h

RESOURCES += \
    resources.qrc
