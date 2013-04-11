TARGET = websocket-chat
TEMPLATE = app

CONFIG += C++11 TUFAO1
QT -= gui

SOURCES += main.cpp \
    webserver.cpp

OTHER_FILES += \
    index.html

HEADERS += \
    webserver.h

RESOURCES += \
    resources.qrc
