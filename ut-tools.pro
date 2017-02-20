#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T16:40:16
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#include(qtpropertybrowser/qtpropertybrowser.pri)

TARGET = ui-tools
TEMPLATE = app



QMAKE_LIBS_OPENGL -= -lGL
QMAKE_CFLAGS += -std=c99

#QMAKE_CXXFLAGS_RELEASE += -g -O3
#QMAKE_CFLAGS_RELEASE += -g -O3
#QMAKE_LFLAGS_RELEASE =

REVISION = $$system(git rev-parse HEAD)
DEFINES += APP_REVISION=\\\"$$REVISION\\\"



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
    xlstool.c \
    ole.c \
    endian.c \
    busyindicator.cpp \
    zoomproject.cpp

    getopt.c

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
    libxls/brdb.c.h \
    config.h \
    busyindicator.h \
    zoomproject.h

FORMS    += mainwindow.ui \
    dialog.ui \
    language.ui \
    confproject.ui \
    globalset.ui \
    griddialog.ui \
    dirdialog.ui \
    find.ui \
    zoomproject.ui
CONFIG += c++11   -fomit-frame-pointer


RESOURCES += \
    icons.qrc

#DISTFILES += \
#    busyindicator.qml

RC_ICONS = logo.ico

DISTFILES += \
    control.json \
    orgtest.json \
    widget.json \
    ut-tools.pro.user.7d87043 \
    backgrounds/ancient.jpg \
    backgrounds/arturodonate_7am_sunrise.jpg \
    backgrounds/arturodonate_skyline.jpg \
    backgrounds/arturodonate_sunrise.jpg \
    backgrounds/blue-marble-west.jpg \
    backgrounds/boat.jpg \
    backgrounds/bobjagendorf_frozen.jpg \
    backgrounds/cloud.jpg \
    backgrounds/comet.jpg \
    backgrounds/earth-horizon.jpg \
    backgrounds/fiji.jpg \
    backgrounds/galaxy-ngc3370.jpg \
    backgrounds/GreenTraditional.jpg \
    backgrounds/helix-nebula.jpg \
    backgrounds/iguazu.jpg \
    backgrounds/inch_beach.jpg \
    backgrounds/inch_beach2.jpg \
    backgrounds/inch_beach3.jpg \
    backgrounds/jupiter.jpg \
    backgrounds/kennymatic_blossoms.jpg \
    backgrounds/kennymatic_edge.jpg \
    backgrounds/kennymatic_funkytown.jpg \
    backgrounds/kennymatic_perspective.jpg \
    backgrounds/kennymatic_sunny.jpg \
    backgrounds/linux_mint_petra.jpg \
    backgrounds/panama.jpg \
    backgrounds/pebbles.jpg \
    backgrounds/rapciu_dawn.jpg \
    backgrounds/rapciu_fresh.jpg \
    backgrounds/rapciu_grass.jpg \
    backgrounds/rapciu_gs1.jpg \
    backgrounds/rapciu_hope.jpg \
    backgrounds/rapciu_winter.jpg \
    backgrounds/road.jpg \
    backgrounds/road2.jpg \
    backgrounds/saltplains.jpg \
    backgrounds/sombrero.jpg \
    backgrounds/storm.jpg \
    backgrounds/stormcoming.jpg \
    backgrounds/valley.jpg \
    backgrounds/whirlpool.jpg \
    backgrounds/Float-into-MATE.png \
    backgrounds/gelsan_green_13.png \
    backgrounds/linux_mint_16.png \
    backgrounds/petra.png \
    backgrounds/Stripes.png \
    icons/align-horizontal.png \
    icons/align-vertical.png \
    icons/checkbox16.png \
    icons/creator_advanceduse.png \
    icons/DeviceDisconnected.png \
    icons/DeviceReadyToUse.png \
    icons/document-open@2x.png \
    icons/document-save-as@2x.png \
    icons/filesave@2x.png \
    images/01.png \
    images/02.png \
    images/03.png \
    images/04.png \
    images/05.png \
    images/06.png \
    images/07.png \
    images/08.png \
    images/09.png \
    images/10.png \
    images/boundary.png \
    images/button2_hovered.png \
    images/button_normal.png \
    images/column-layouts-icon.png \
    images/creator_android_tutorial_ex_app.png \
    images/Desktop.png \
    images/desktopdevice@2x.png \
    images/frame.png \
    images/gridview-icon.png \
    images/icon_256x256@2x.png \
    images/icon_512x512.png \
    images/item.png \
    images/mode_debug@2x.png \
    images/mode_help@2x.png \
    images/panel_button_checked@2x.png \
    images/penguin.png \
    images/progress-fill.png \
    images/QtAndroid.png \
    images/QtBareMetal.png \
    images/row-layouts-icon.png \
    images/selectedrow.png \
    images/selection@2x.png \
    images/targetpanel_gradient.png \
    images/targetunselected.png \
    images/todo.png \
    images/window-new.png \
    images/window-new@2x.png \
    icons/logo.ico \
    logo.ico \
    LICENSE \



unix:!macx: LIBS += -L$$PWD/ -lffmpeg-linux -ldl -lxcb -lxcb-shm -lxcb \
                             -lxcb-xfixes -lxcb-render -lxcb-shape \
                             -lxcb -lxcb-shape -lxcb -lX11 -lm -llzma -lz -pthread

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

unix:!macx: PRE_TARGETDEPS += $$PWD/libffmpeg-linux.a


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lffmpeg-win -lgdi32  -liconv -lsecur32 -lm -llzma -lz -lpsapi -ladvapi32 -lshell32 -static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lffmpeg-win -lgdi32  -liconv -lsecur32 -lm -llzma -lz -lpsapi -ladvapi32 -lshell32 -static

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libffmpeg-win.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libffmpeg-win.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/ffmpeg-win.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/ffmpeg-win.lib
