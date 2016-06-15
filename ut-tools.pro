#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T16:40:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ut-tools
TEMPLATE = app



QMAKE_LIBS_OPENGL -= -lGL

SOURCES += main.cpp\
        mainwindow.cpp \
    module.cpp \
    handlejson.cpp \
    imagefiledialog.cpp \
    compoentcontrols.cpp \
    treedock.cpp

HEADERS  += mainwindow.h \
    module.h \
    handlejson.h \
    imagefiledialog.h \
    compoentcontrols.h \
    treedock.h

FORMS    += mainwindow.ui
CONFIG += c++11


RESOURCES += \
    ut-tools.qrc

