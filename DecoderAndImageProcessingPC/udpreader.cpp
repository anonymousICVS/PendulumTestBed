#include "udpreader.h"
#include <iostream>
#include <QByteArray>
#include <QString>
#include <QHostAddress>

#include <stdio.h>
#include <chrono>

using namespace std;

UdpReader::UdpReader(bool videoOrAngle)
    :voa(videoOrAngle)
{
    framecounter = 0;
    linesize = (int *)malloc(sizeof(int)*1);
    linesize[0] = 3*160;
    start_time = std::chrono::high_resolution_clock::now();
    connect(this, SIGNAL(readyRead()), this, SLOT(readTheData()));
}

UdpReader::~UdpReader()
{
    std::cout << "UDP DESTRUCTOR CALLED" << std::endl;
    this->close();
}

void UdpReader::initReader()
{
    if(voa){
        this->bind(QHostAddress("10.152.4.124"), 5000);
    }else{
        this->bind(QHostAddress("192.168.1.100"), 8889);
    }

//    this->bind(QHostAddress("10.152.13.92"), 5000);
}

void UdpReader::readTheData()
{



    while (this->hasPendingDatagrams()) {
        framecounter++;
        QByteArray datagram;
        datagram.resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        this->readDatagram(datagram.data(), datagram.size(),
                           &sender, &senderPort);
//        cout << framecounter << endl;
//        if((framecounter<<25) == 0){
//            auto end_time = std::chrono::high_resolution_clock::now();
//            std::cout << "Current Frame Rate: " << (double)framecounter*1000/(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()) << std::endl;
//        }

//        cout << datagram.size() << endl;
        if(voa){
            decode(datagram.size(), reinterpret_cast<uchar*>(datagram.data()));
        }else{
            TriggerAngleRecording(datagram.size(), reinterpret_cast<char*>(datagram.data()));
        }

//        display(reinterpret_cast<uchar*>(datagram.data()), linesize);
    }
}
