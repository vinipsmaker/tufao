TARGET = Tufao
TEMPLATE = lib

DEFINES += TUFAO_LIBRARY

# Qt Creator linking

QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):error(QTC_SOURCE env variable not defined)

IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):error(QTC_BUILD env variable not defined)

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\Nokia\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/Nokia/qtcreator" or "~/.local/share/Nokia/qtcreator" on Linux
##    "~/Library/Application Support/Nokia/Qt Creator" on Mac
# USE_USER_DESTDIR = yes

PROVIDER = TufaoTeam

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/coreplugin/coreplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/projectexplorer/projectexplorer.pri)

LIBS += -L$$IDE_PLUGIN_PATH/Nokia

# Tufao files

OTHER_FILES = Tufao.pluginspec

RESOURCES += \
    resources.qrc

FORMS += \
    projecttypewizardpage.ui

SOURCES += tufaoplugin.cpp \
    tufaowizard.cpp \
    projecttypewizardpage.cpp

HEADERS += tufaoplugin.h\
        tufao_global.h\
        tufaoconstants.h \
    tufaowizard.h \
    projecttypewizardpage.h
