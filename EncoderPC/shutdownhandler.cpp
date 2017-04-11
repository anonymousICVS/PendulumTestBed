#include "shutdownhandler.h"

#include <iostream>

#include "highgui.hpp"

using namespace std;

shutdownHandler::shutdownHandler(QCoreApplication *a)
    :m_shutdown(false), m_a(a), m_camDown(false), m_portDown(false), m_netStreamerDown(false), m_decoderDown(false), m_counterDown(false), m_encoderDown(false)
{
//    cv::namedWindow("current frame before encoding", cv::WINDOW_AUTOSIZE);
//    cv::namedWindow("diff of latest 2 frames to detect LED-ON-Event", cv::WINDOW_AUTOSIZE);
}

shutdownHandler::~shutdownHandler()
{
    cv::destroyAllWindows();
    cout << "ready to close everything" << endl;
    m_a->quit();
}

void shutdownHandler::shutdown()
{
    cout << "SHUTDOWN SHUTDOWNHANDLER" << endl;
    m_shutdown = true;
    emit killRest();
}

void shutdownHandler::cameraDown()
{
    m_camDown = true;
    this->closeQApp();
}

void shutdownHandler::serialPortDown()
{
    m_portDown = true;
    this->closeQApp();
}

void shutdownHandler::networkStreamerDown()
{
    m_netStreamerDown = true;
    this->closeQApp();
}

void shutdownHandler::decoderDown()
{
    m_decoderDown = true;
    this->closeQApp();
}

void shutdownHandler::counterDown()
{
    m_counterDown = true;
    this->closeQApp();
}

void shutdownHandler::encoderDown()
{
    m_encoderDown = true;
    this->closeQApp();
}

void shutdownHandler::closeQApp()
{
    if(m_camDown && m_portDown && m_netStreamerDown && m_decoderDown && m_counterDown && m_encoderDown)
    {
        //cv::destroyAllWindows();
        //this->deleteLater();
        deleteLater();
    }
}
