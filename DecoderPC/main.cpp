#include <QCoreApplication>

#include <measuretime.h>
#include <decoder.h>
#include <udpreader.h>
#include <yuvviewer.h>
#include <processing.h>
#include <botcommunication.h>

#include <QThread>
#include <QFile>
#include <QElapsedTimer>

#include "core.hpp"
#include "highgui.hpp"
#include "videoio.hpp"
#include "imgproc.hpp"
#include "core.hpp"
#include "highgui.hpp"
#include "videoio.hpp"
#include "imgproc.hpp"
#include "imgcodecs.hpp"

#define XRES 640
#define YRES 480

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Decoder *dec = new Decoder(XRES, YRES, 0);
    QThread *threadDec = new QThread();
    threadDec->setPriority(QThread::HighPriority);

    //inits processing
    Processing *proc = new Processing();
    QThread *threadProc = new QThread();

    //inits sender
    BotCommunication *botcom = new BotCommunication();
    QThread *threadBotCom = new QThread();

    YuvViewer *yuv = new YuvViewer(XRES, YRES, 0, 0);
    QThread *threadYuv = new QThread();
    threadYuv->setPriority(QThread::HighPriority);

    UdpReader *udp = new UdpReader(1); //For reading the video data
    udp->initReader();
    UdpReader *udp2 = new UdpReader(0); //For the start/stop signals from the pendulum
    udp2->initReader();

    QObject::connect(udp, SIGNAL(decode(int,uchar*)), dec, SLOT(decode(int,uchar*)));
    QObject::connect(udp2, SIGNAL(TriggerAngleRecording(int,char*)), proc, SLOT(TriggerRecording(int,char*)));

    dec->moveToThread(threadDec);
    yuv->moveToThread(threadYuv);
    proc->moveToThread(threadProc);
    botcom->moveToThread(threadBotCom);

    QObject::connect(threadDec, SIGNAL(started()), dec, SLOT(process()));
    QObject::connect(yuv, SIGNAL(finished()), threadDec, SLOT(quit()));
    QObject::connect(yuv, SIGNAL(finished()), dec, SLOT(deleteLater()));
    QObject::connect(threadYuv, SIGNAL(finished()), threadDec, SLOT(deleteLater()));

    QObject::connect(threadYuv, SIGNAL(started()), yuv, SLOT(process()));
    QObject::connect(yuv, SIGNAL(finished()), threadYuv, SLOT(quit()));
    QObject::connect(yuv, SIGNAL(finished()), yuv, SLOT(deleteLater()));
    QObject::connect(threadYuv, SIGNAL(finished()), threadYuv, SLOT(deleteLater()));

    QObject::connect(threadBotCom, SIGNAL(started()), botcom, SLOT(process()));
    QObject::connect(botcom, SIGNAL(finished()), threadBotCom, SLOT(quit()));
    QObject::connect(botcom, SIGNAL(finished()), botcom, SLOT(deleteLater()));
    QObject::connect(threadBotCom, SIGNAL(finished()), threadBotCom, SLOT(deleteLater()));

    QObject::connect(yuv,SIGNAL(finished()),udp,SLOT(deleteLater()));
    QObject::connect(yuv,SIGNAL(finished()),udp2,SLOT(deleteLater()));
    QObject::connect(proc, SIGNAL(SendAngle(double,double)), botcom, SLOT(SendAngle(double,double)));
    QObject::connect(dec, SIGNAL(processFrame()), proc, SLOT(ProcessFrame()));
    QObject::connect(dec, SIGNAL(copyFrame(cv::Mat*)), proc, SLOT(CopyFrame(cv::Mat*)),Qt::BlockingQueuedConnection);

    QObject::connect(dec, SIGNAL(sendYuvFrame(uchar**, int*)), yuv, SLOT(displayImage(uchar**, int*)));

//    threadYuv->start();
    threadDec->start();
    threadProc->start();
    threadBotCom->start();

//    std::getchar();

    return a.exec();
}


