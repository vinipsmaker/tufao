TARGET = test
TEMPLATE = app

CONFIG += TUFAO
QT     -= gui

OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = bin

# TIMEOUT_CLIENT_TIME MUST be less than the Tufão timeout
# TIMEOUT_SERVER_TIME MUSTN'T be less than the Tufão timeout
DEFINES += WAIT_TIME=1500 TIMEOUT_CLIENT_TIME=100000 TIMEOUT_SERVER_TIME=130000

SOURCES += src/main.cpp \
    src/stringlistsocket.cpp \
    src/test1.cpp \
    src/test2.cpp \
    src/testsguide.cpp \
    src/abstracttest.cpp \
    src/test3.cpp \
    src/test4.cpp \
    src/test6.cpp \
    src/test8.cpp \
    src/test10.cpp \
    src/test9.cpp

HEADERS += src/stringlistsocket.h \
    src/test1.h \
    src/test2.h \
    src/testsguide.h \
    src/abstracttest.h \
    src/test3.h \
    src/test4.h \
    src/test6.h \
    src/test8.h \
    src/test10.h \
    src/test9.h
