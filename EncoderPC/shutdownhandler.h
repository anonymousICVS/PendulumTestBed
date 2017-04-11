#ifndef SHUTDOWNHANDLER_H
#define SHUTDOWNHANDLER_H

#include <QObject>
#include <QCoreApplication>

class shutdownHandler : public QObject
{

    Q_OBJECT

public:
    shutdownHandler(QCoreApplication *a);
    ~shutdownHandler();
    bool m_shutdown;

public slots:
    void shutdown();
    void cameraDown();
    void serialPortDown();
    void networkStreamerDown();
    void decoderDown();
    void counterDown();
    void encoderDown();

signals:
    void killRest();

private:
    QCoreApplication *m_a;
    bool m_camDown;
    bool m_portDown;
    bool m_netStreamerDown;
    bool m_decoderDown;
    bool m_counterDown;
    bool m_encoderDown;
    void closeQApp();
};

#endif // SHUTDOWNHANDLER_H
