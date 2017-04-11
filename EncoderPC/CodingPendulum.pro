#-------------------------------------------------
#
# Project created by QtCreator 2016-07-06T09:21:55
#
#-------------------------------------------------

QT       += core network serialport

QT       -= gui

TARGET = CodingPendulum
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++14

TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /home/industry/Downloads
INCLUDEPATH += /usr/include/python2.7
LIBS += -lopencv_highgui -lopencv_videoio -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lm3api -L"/usr/local/lib" -lx264 -lswscale -lavformat -lavcodec -lavutil -lswresample -ldl -lm -lz -lpthread -lSDL2 -lva -lpython2.7

SOURCES += main.cpp \
    cameraframegrabber.cpp \
    encoder.cpp \
    networkstreamer.cpp \
    shutdownhandler.cpp \
    frameprocessor.cpp

HEADERS += \
    cameraframegrabber.h \
    encoder.h \
    networkstreamer.h \
    shutdownhandler.h \
    frameprocessor.h

OTHER_FILES += \
    merger.py
