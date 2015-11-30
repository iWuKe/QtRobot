#-------------------------------------------------
#
# Project created by QtCreator 2014-08-30T21:14:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network printsupport


INCLUDEPATH += /usr/local/include/opencv2/

include(third_party/common/common.pri)
include(third_party/svgbutton/svgbutton.pri)
include(third_party/svgtoggleswitch/svgtoggleswitch.pri)
include(third_party/svgdialgauge/svgdialgauge.pri)
include(third_party/multislider/multislider.pri)

TARGET = robotgui
TEMPLATE = app

LIBS  += -lusb-1.0
LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc
CONFIG += debug

SOURCES += main.cpp\
        mainwindow.cpp \
    qlibusb.cpp \
    threadEthernet.cpp \
    qcustomplot.cpp \
    capturethread.cpp

HEADERS  += mainwindow.h \
    protocol.h \
    qlibusb.h \
    threadEthernet.h \
    qcustomplot.h \
    capturethread.h

FORMS    += mainwindow.ui

linux-* {
    target.path = ./
    INSTALLS += target
}

RESOURCES += \
             third_party/skins/beryl_multislider.qrc          \
             third_party/skins/beryl_svgslideswitch.qrc       \
             third_party/skins/beryl_scrolldial.qrc           \
             third_party/skins/beryl_5waybutton.qrc           \
             third_party/skins/beryl_scrollwheel.qrc          \
             third_party/skins/beryl_svgtoggleswitch.qrc      \
             third_party/skins/beryl_svgbutton.qrc            \
             third_party/skins/berylsquare_svgbutton.qrc      \
             third_party/skins/berylsquare_svgtoggleswitch.qrc\
             third_party/skins/amperemeter_svgdialgauge.qrc   \
             third_party/skins/thermometer_svgdialgauge.qrc   \
             third_party/skins/tachometer_svgdialgauge.qrc    \
             resources.qrc
