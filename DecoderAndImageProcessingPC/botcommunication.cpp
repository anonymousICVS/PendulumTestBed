#include "botcommunication.h"
#include "measuretime.h"

#include <QtNetwork/QNetworkInterface>
#include <QString>
#include <QHostAddress>
#include <QDataStream>

extern MeasureTime mytime;

BotCommunication::BotCommunication()
{



}


BotCommunication::~BotCommunication()
{
    udpsock->close();
    delete udpsock;
}

void BotCommunication::process()
{
    //printf("Bound!\n");
    udpsock = new QUdpSocket();
    /*
    if (udpsock->bind(QHostAddress::LocalHost, 7755))
    {
        printf("Bound!\n");
    }
    else
    {
        printf("Not Bound!\n");
    }
    */
    QString sendString = "START";
    QByteArray sendData(sendString.toStdString().c_str(), sendString.toStdString().length());
    //std::cout << sendData.data() << std::endl;
    udpsock->writeDatagram(sendData, QHostAddress(BOTADDRESS), BOTPORT);
    //while (!udpsock->hasPendingDatagrams()){}
    while (udpsock->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpsock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsock->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        std::cout << datagram.data() << std::endl;
    }
}

void BotCommunication::SendAngle(double Angle, double Speed)
{
    long long time;
    /*
    while (udpsock->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpsock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsock->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        std::cout << "POSSIBLY LOST" << std::endl;
    }
    */




    //DO SOME PROCESSING WITH THE ANGLE
    QByteArray sendbuffer;
    QDataStream sendstream(&sendbuffer, QIODevice::WriteOnly);
    sendstream << float(Angle) << float(Speed);




    //mytime.StartMeasure(14);

    udpsock->writeDatagram(sendbuffer, QHostAddress(BOTADDRESS), BOTPORT);

//    std::cout << "Sent angle!" << std::endl;



    /*
    while (!udpsock->hasPendingDatagrams()){}
//    printf("STEP 2\n");

    while (udpsock->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpsock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsock->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        //std::cout << datagram.data() << std::endl;
    }
    time = mytime.StopMeasure(14);
    printf("Round trip time: %d microseconds\n",int(time));
    */
}
