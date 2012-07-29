TARGET = ShugoConsole

CONFIG = qt windows thread resources
QT = core gui network

HEADERS += ./ntdll.h \
    ./qtlockedfile.h \
    ./mainwindow.h \
    ./qtsinglecoreapplication.h \
    ./qtsingleapplication.h \
    ./qtlocalpeer.h
SOURCES += ./main.cpp \
    ./mainwindow.cpp \
    ./ntdll.cpp \
    ./qtlocalpeer.cpp \
    ./qtsingleapplication.cpp \
    ./qtsinglecoreapplication.cpp
RESOURCES += ./ShugoConsole.qrc
RC_FILE += ./ShugoConsole.rc




