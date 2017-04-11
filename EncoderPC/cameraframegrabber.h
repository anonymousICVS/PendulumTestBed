#ifndef CAMERAFRAMEGRABBER_H
#define CAMERAFRAMEGRABBER_H

#include <QObject>
#include <QFile>

#include "core.hpp"
#include "highgui.hpp"
#include "videoio.hpp"
#include "imgproc.hpp"
#include "imgcodecs.hpp"
#include <stdio.h>
#include <iostream>
#include <string>
#include <m3api/xiApi.h>
#include <chrono>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include <shutdownhandler.h>

#include <QDebug>

using namespace cv;
using namespace std;

class cameraFrameGrabber : public QObject
{

    Q_OBJECT

public:
    cameraFrameGrabber(shutdownHandler *shH, int x, int y, int xoffset, int yoffset, int exposure, bool autoExposureGain, int imgFormat, int tmin, int tmax, double thr, int fps, QFile *file);
    ~cameraFrameGrabber();

public slots:
    void process();

signals:
    void finished();
    void down();
    void processFrame(uchar *,int, int);
    void closeEncoder();
    void sendRawFrame(int, uchar*);


private:
    SwsContext *conv;
    VideoCapture *m_capture;
    Mat m_currentFrame;
    Mat m_currentFrame_grey;
    Mat m_oldFrame_grey;
    Mat m_differenceImg;
    bool m_first;
    shutdownHandler *m_shH;
    int m_x;
    int m_y;
    int m_xoffset;
    int m_yoffset;
    int m_fps;
    int m_exposure;
    bool m_autoExposureGain;
    int m_imgFormat;
    double m_thr;
    int m_tmin;
    int m_tmax;
    QFile *m_file;

    int fd, RTS_flag, set_flag;
};

#endif // CAMERAFRAMEGRABBER_H
