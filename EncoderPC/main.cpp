#include <QCoreApplication>
#include <QThread>
#include <QElapsedTimer>

#include <Python.h>

#include <cameraframegrabber.h>
#include <frameprocessor.h>
#include <encoder.h>
#include <networkstreamer.h>



//Camera Parameters
#define FPS 100
#define X_RES 640
#define Y_RES 480
#define X_OFF 704
#define Y_OFF 304
#define AUTO_EXP_GAIN false         // we want to control exposure on our own to get the desired fixed framerates
#define IMG_FORMAT 0                // 5 is RAW8 (1channel 0..255) whereas 2 is RGB24 (3channels 0..255), 0 is MONO8 (bayer preprocessing) ---> 5 (RAW8) allows highest framerates (still depending on resolution)
#define THR 12                       //The threshold for the frame selection
#define T_MIN 0                     
#define T_MAX 0                    

int main(int argc, char* argv[])
{
    QCoreApplication *qCore = new QCoreApplication(argc, argv);

    //For logging:
    QFile *file;
    file = new QFile("log_cam.csv");
    if(!(file->open(QFile::WriteOnly))){
        std::cout << "Error opening the Log file!" << std::endl;
    }
    QFile *file2;
    file2 = new QFile("log_enc.csv");
    if(!(file2->open(QFile::WriteOnly))){
        std::cout << "Error opening the Log2 file!" << std::endl;
    }

    QElapsedTimer *timer = new QElapsedTimer();
    shutdownHandler *shH = new shutdownHandler(qCore);

    int fps = FPS;
    int exposure = 1000000/fps;
    cameraFrameGrabber *cam = new cameraFrameGrabber(shH, X_RES, Y_RES, X_OFF, Y_OFF, exposure, AUTO_EXP_GAIN, IMG_FORMAT, T_MIN, T_MAX, 7, FPS, file);
    QThread *threadCam = new QThread();
    threadCam->setPriority(QThread::HighPriority);
    threadCam->setObjectName("xiQ_FS");

    //inits fixed-size encoders
    Encoder *enc = new Encoder(X_RES, Y_RES, fps, T_MAX, 0, file2);
    QThread *threadEnc = new QThread();
    threadEnc->setPriority(QThread::HighPriority);
    threadEnc->setObjectName("xiQ_Enc");
    //inits networkstreamer
//    NetworkStreamer *ns = new NetworkStreamer();
//    QThread *threadNs = new QThread();




    //inits frame processor
    //    FrameProcessor *fp = new FrameProcessor();
    //    QThread *threadFp = new QThread();


    //move all the instances into their own thread
    cam->moveToThread(threadCam);
    enc->moveToThread(threadEnc);
//    ns->moveToThread(threadNs);
    //    fp->moveToThread(threadFp);


    QObject::connect(threadCam, SIGNAL(started()), cam, SLOT(process()));
    QObject::connect(cam, SIGNAL(finished()), threadCam, SLOT(quit()));
    QObject::connect(cam, SIGNAL(finished()), cam, SLOT(deleteLater()));
    QObject::connect(threadCam, SIGNAL(finished()), threadCam, SLOT(deleteLater()));

    //    QObject::connect(threadFp, SIGNAL(started()), fp, SLOT(process()));
    //    QObject::connect(fp, SIGNAL(finished()), threadFp, SLOT(quit()));
    //    QObject::connect(fp, SIGNAL(finished()), fp, SLOT(deleteLater()));
    //    QObject::connect(threadFp, SIGNAL(finished()), threadFp, SLOT(deleteLater()));

    QObject::connect(threadEnc, SIGNAL(started()), enc, SLOT(process()));
    QObject::connect(enc, SIGNAL(finished()), threadEnc, SLOT(quit()));
    QObject::connect(enc, SIGNAL(finished()), enc, SLOT(deleteLater()));
    QObject::connect(threadEnc, SIGNAL(finished()), threadEnc, SLOT(deleteLater()));

//    QObject::connect(threadNs, SIGNAL(started()), ns, SLOT(process()));
//    QObject::connect(ns, SIGNAL(finished()), threadNs, SLOT(quit()));
//    QObject::connect(ns, SIGNAL(finished()), ns, SLOT(deleteLater()));
//    QObject::connect(threadNs, SIGNAL(finished()), threadNs, SLOT(deleteLater()));


    QObject::connect(cam, SIGNAL(processFrame(uchar*,int, int)), enc, SLOT(encodeSlot(uchar*,int, int)));
    //    QObject::connect(cam, SIGNAL(processFrame(uchar*)), fp, SLOT(receiveFrame(uchar*)));
    //    QObject::connect(fp, SIGNAL(sendFrame(uchar*)), enc, SLOT(encodeSlot(uchar*)));
//    QObject::connect(enc, SIGNAL(writeToNetwork(int, u_int8_t *)), ns, SLOT(writeOutput(int, u_int8_t *)));



    //start threads
    threadCam->start();
    threadEnc->start();
//    threadNs->start();

    //User presses enter: shutdown everything
    std::getchar();
    shH->m_shutdown = true;


    timer->restart();
    while(1) {
        if(timer->nsecsElapsed() > 1000000000)
        {
            delete timer;
            break;
        }
    }
    file2->close();



    if (threadCam->isRunning())
    {
        printf("FINISHED\n");
    }
    //starting QT Event Loop
//        return qCore->exec();
}

