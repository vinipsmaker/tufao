#-------------------------------------------------
#
# Project created by QtCreator 2011-10-03T22:03:33
#
#-------------------------------------------------

QT       += network
QT       -= gui

TARGET = tufao

TEMPLATE = lib

CONFIG += dll

VERSION = 0.1.0

DEFINES += TUFAO_LIBRARY

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE6DE46D1
    TARGET.CAPABILITY =
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = tufao.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

# Install data
PREFIX = /usr/local

headers.path = $$PREFIX/include/tufao
headers.files = src/httpserver.h\
    src/tufao_global.h \
    src/httpserverrequest.h \
    src/httpserverresponse.h \
    src/httpsserver.h \
    src/url.h \
    src/querystring.h

documentation.path = $$PREFIX/share/doc/qt/qch
documentation.files = doc/html/tufao.qch
documentation.extra = doxygen

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = $$PREFIX/lib
    }
}

INSTALLS += target headers documentation

# Project files
SOURCES += src/httpserver.cpp \
    src/httpserverrequest.cpp \
    src/httpserverresponse.cpp \
    src/httpsserver.cpp \
    src/url.cpp \
    src/querystring.cpp \
    src/priv/tcpserverwrapper.cpp \
    src/priv/reasonphrase.cpp \
    src/priv/http_parser.cpp

HEADERS += src/httpserver.h\
    src/tufao_global.h \
    src/httpserverrequest.h \
    src/httpserverresponse.h \
    src/httpsserver.h \
    src/url.h \
    src/querystring.h \
    src/priv/tcpserverwrapper.h \
    src/priv/httpserver.h \
    src/priv/httpserverrequest.h \
    src/priv/caseinsensitivebytearraymap.h \
    src/priv/httpsserver.h \
    src/priv/httpserverresponse.h \
    src/priv/reasonphrase.h \
    src/priv/http_parser_qt_helper.h \
    src/priv/http_parser.h \
    src/priv/url.h
