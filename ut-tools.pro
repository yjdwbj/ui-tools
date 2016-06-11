#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T16:40:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ut-tools
TEMPLATE = app




SOURCES += main.cpp\
        mainwindow.cpp \
    dragwidget.cpp \
    module.cpp \
    handlejson.cpp

HEADERS  += mainwindow.h \
    dragwidget.h \
    module.h \
    handlejson.h

FORMS    += mainwindow.ui
CONFIG += c++11

RESOURCES += \
    ut-tools.qrc
LIBS +=  -L /usr/local/lib -lglapi
