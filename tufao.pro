#-------------------------------------------------
#
# Project created by QtCreator 2011-10-03T22:03:33
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = tufao
TEMPLATE = lib

DEFINES += TUFAO_LIBRARY

SOURCES += httpserver.cpp \
    httpserverrequest.cpp \
    httpserverresponse.cpp

HEADERS += httpserver.h\
    tufao_global.h \
    httpserverrequest.h \
    httpserverresponse.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE6DE46D1
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = tufao.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    Doxyfile
