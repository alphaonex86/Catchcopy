#-------------------------------------------------
#
# Project created by QtCreator 2011-06-28T21:21:08
#
#-------------------------------------------------

QT       += core gui

TARGET = catchcopy-cpp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ClientCatchcopy.cpp

HEADERS  += mainwindow.h \
    ClientCatchcopy.h

FORMS    += mainwindow.ui

LIBS+=-lws2_32
