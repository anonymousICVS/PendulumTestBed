#-------------------------------------------------
#
# Project created by QtCreator 2016-07-06T10:09:40
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = CodingPendulumReceiver
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++14

TEMPLATE = app


SOURCES += main.cpp \
    measuretime.cpp \
    udpreader.cpp \
    yuvviewer.cpp \
    decoder.cpp \
    botcommunication.cpp \
    processing.cpp

INCLUDEPATH += /home/christoph/ffmpeg_build/include
INCLUDEPATH += /usr/local/include/opencv2
LIBS += -lopencv_highgui -lopencv_videoio -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -L/home/christoph/ffmpeg_build/lib -L"/usr/local/lib"  -lswscale -lavformat -lavcodec -lavutil -lswresample -ldl -lm -lz -lpthread -lSDL2 -lX11 -lx264 -lva


HEADERS += \
    measuretime.h \
    udpreader.h \
    yuvviewer.h \
    decoder.h \
    botcommunication.h \
    processing.h \
    constants.h
