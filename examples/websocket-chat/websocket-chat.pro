TARGET = websocket-chat
TEMPLATE = app

CONFIG += TUFAO
QT -= gui

SOURCES += main.cpp \
    webserver.cpp

OTHER_FILES += \
    index.html

HEADERS += \
    webserver.h

RESOURCES += \
    resources.qrc
