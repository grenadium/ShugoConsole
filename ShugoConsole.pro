QT       += core gui widgets
CONFIG   += windows resources

TARGET = ShugoConsole
TEMPLATE = app

win32:LIBS += ntdll.lib

# DEFINES += ENABLE_CONSOLE_ACTIVATION

SOURCES += main.cpp\
        shugoconsolewidget.cpp \
    aionprocessworker.cpp \
    win64utils.cpp \
    remotememorylookup.cpp \
    memorypattern.cpp \
    crycvar.cpp \
    processutils.cpp \
    processlistmodel.cpp \
    variableeditor.cpp

HEADERS  += shugoconsolewidget.h \
    ntdll.h \
    aionprocessworker.h \
    win64utils.h \
    remotememorylookup.h \
    memorypattern.h \
    crycvar.h \
    processutils.h \
    processlistmodel.h \
    variableeditor.h

FORMS    += shugoconsolewidget.ui

RESOURCES += ShugoConsole.qrc

RC_FILE += ShugoConsole.rc

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

