#ifndef UDPREADER_H
#define UDPREADER_H

#include <QUdpSocket>
#include <chrono>


/**
 * @brief The UdpReader class listens on the udp socket and receives the packages sent by frameskipper.
 * The sideinformation of the fixed-size windows and roi is extracted from the packet and then both the sideinformation and the encoded data is sent to the correct dec
 */
class UdpReader : public QUdpSocket
{
    struct bytestruct {
        uchar x_byte0;
        uchar x_byte1;
        uchar y_byte0;
        uchar y_byte1;
        uchar width_byte0;
        uchar width_byte1;
        uchar height_byte0;
        uchar height_byte1;
    };

    struct shortstruct {
        short x;
        short y;
        short width;
        short height;
        short roi_x;
        short roi_y;
        short roi_width;
        short roi_height;
    };

    union uni {
        bytestruct bs;
        shortstruct ss;
    };

    Q_OBJECT

public:
    /**
     * @brief UdpReader constructs the instance
     */
    UdpReader(bool videoOrAngle);
    ~UdpReader();

    /**
     * @brief initReader initializes the udp socket
     */
    void initReader();
    int framecounter;
    int *linesize;
    std::chrono::_V2::system_clock::time_point start_time;

signals:
    /**
     * @brief decode sends the encoded data along with the sideinformation to the decoder
     * @param size the length of the encoded data packet
     * @param data the pointer to the data of the packet
     */
    void decode(int size, uchar* data);
    void display(uchar*, int*);
    void TriggerAngleRecording(int size, char* data);

public slots:
    void readTheData();

private:
    bool voa;
};

#endif // UDPREADER_H
