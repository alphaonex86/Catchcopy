#-------------------------------------------------
#
# Project created by QtCreator 2010-11-14T13:02:19
#
#-------------------------------------------------

QT       += core gui network

TARGET = test-server-catchcopy
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    catchcopy-api-0002/ServerCatchcopy.cpp \
    catchcopy-api-0002/ExtraSocketCatchcopy.cpp

HEADERS  += mainwindow.h \
    catchcopy-api-0002/ServerCatchcopy.h \
    catchcopy-api-0002/ExtraSocketCatchcopy.h

FORMS    += mainwindow.ui
win32:RC_FILE += resources-windows.rc
RESOURCES += resources.qrc
