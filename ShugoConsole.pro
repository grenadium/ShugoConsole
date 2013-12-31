QT       += core gui widgets
CONFIG   += windows resources

TARGET = ShugoConsole
TEMPLATE = app

# Disable automatic manifest generation
CONFIG -= embed_manifest_exe

win32-g++ {
    LIBS += -lntdll
    QMAKE_CXXFLAGS += -std=c++11
}

win32-msvc* {
    LIBS += ntdll.lib
}

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
    variableeditor.cpp \
    wintaskscheduler.cpp

HEADERS  += shugoconsolewidget.h \
    ntdll.h \
    aionprocessworker.h \
    win64utils.h \
    remotememorylookup.h \
    memorypattern.h \
    crycvar.h \
    processutils.h \
    processlistmodel.h \
    variableeditor.h \
    wintaskscheduler.h

FORMS    += shugoconsolewidget.ui

RESOURCES += ShugoConsole.qrc

RC_FILE += ShugoConsole.rc

OTHER_FILES += \
    ShugoConsole.rc \
    ShugoConsole.exe.manifest

