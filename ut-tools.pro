#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T16:40:16
#
#-------------------------------------------------

QT       += core gui quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ut-tools
TEMPLATE = app



QMAKE_LIBS_OPENGL -= -lGL

SOURCES += main.cpp\
        mainwindow.cpp \
    module.cpp \
    compoentcontrols.cpp \
    treedock.cpp \
    scenesscreen.cpp \
    sizehandlerect.cpp \
    formresizer.cpp \
    canvasmanager.cpp \
    pageview.cpp \
    mydialog.cpp \
    xls.c \
    ole.c \
    getopt.c \
    endian.c \
    xlstool.c

HEADERS  += mainwindow.h \
    module.h \
    compoentcontrols.h \
    treedock.h \
    scenesscreen.h \
    sizehandlerect.h \
    formresizer.h \
    canvasmanager.h \
    pageview.h \
    mydialog.h \
    core_def.h \
    libxls/xlstypes.h \
    libxls/xlstool.h \
    libxls/xlsstruct.h \
    libxls/xls.h \
    libxls/ole.h \
    libxls/endian.h \
    libxls/brdb.h \
    libxls/brdb.c.h

FORMS    += mainwindow.ui \
    dialog.ui \
    language.ui
CONFIG += c++11


RESOURCES += \
    icons.qrc

DISTFILES += \
    busyindicator.qml

