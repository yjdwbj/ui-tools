#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T16:40:16
#
#-------------------------------------------------

QT       += core gui quick axcontainer

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
    mydialog.cpp

HEADERS  += mainwindow.h \
    module.h \
    compoentcontrols.h \
    treedock.h \
    config.h \
    scenesscreen.h \
    sizehandlerect.h \
    formresizer.h \
    canvasmanager.h \
    pageview.h \
    mydialog.h

FORMS    += mainwindow.ui \
    dialog.ui
CONFIG += c++11


RESOURCES += \
    icons.qrc

DISTFILES += \
    busyindicator.qml

