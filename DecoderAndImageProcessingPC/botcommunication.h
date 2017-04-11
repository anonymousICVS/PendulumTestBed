#ifndef BOTCOMMUNICATION_H
#define BOTCOMMUNICATION_H

#endif // BOTCOMMUNICATION_H



#include "constants.h"
#include <QUdpSocket>
#include <QObject>

class BotCommunication : public QObject
{
    Q_OBJECT

public slots:
    void SendAngle(double Angle, double speed);
    void process();

signals:
    void finished();

public:
    BotCommunication();
    ~BotCommunication();


private:
    QUdpSocket *udpsock;
};
