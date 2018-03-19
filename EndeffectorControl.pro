#-------------------------------------------------
#
# Project created by QtCreator 2017-10-13T08:45:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EndeffectorControl
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += thread
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#LIBS += \
#       easyusb_card_dll.lib \
#        EposCmd.lib

LIBS += -LE:/Shepherd-2017/AcupunctureRobot/EndeffectorControl -leasyusb_card_dll
LIBS += -LE:/Shepherd-2017/AcupunctureRobot/EndeffectorControl -lEposCmd
LIBS += -LE:/Shepherd-2017/AcupunctureRobot/EndeffectorControl -lhd

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    sensor.cpp \
    epos.cpp \
    motor.cpp \
    motorthreads.cpp

HEADERS += \
        mainwindow.h \
    easyusb_card_dll.h \
    sensor.h \
    Definitions.h \
    epos.h \
    motor.h \
    motorthreads.h \
    hd.h \
    hdCompilerConfig.h \
    hdDefines.h \
    hdDevice.h \
    hdExport.h \
    hdScheduler.h

FORMS += \
        mainwindow.ui
