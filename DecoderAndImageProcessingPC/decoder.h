#ifndef DECODER_H
#define DECODER_H

#include <QFile>
#include <QTextStream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <fcntl.h>   	 /* File Control Definitions           */
#include <termios.h>	 /* POSIX Terminal Control Definitions */
#include <unistd.h> 	 /* UNIX Standard Definitions 	       */
#include <errno.h>   	 /* ERROR Number Definitions           */
#include <sys/ioctl.h>   /* ioctl()                            */

#include "core.hpp"
#include "highgui.hpp"
#include "videoio.hpp"
#include "imgproc.hpp"
#include "core.hpp"
#include "highgui.hpp"
#include "videoio.hpp"
#include "imgproc.hpp"
#include "imgcodecs.hpp"

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif


#include <QObject>

/**
 * @brief The Decoder class receives the content of an encoder's NAL package and reconstructs a YUV-image.
 * The data of the image is sent to the YUV-viewer class.
 */
class Decoder : public QObject
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
    };

    union uni {
        bytestruct bs;
        shortstruct ss;
    };

    Q_OBJECT

public:
    /**
     * @brief Decoder constructs the instance with the given dimensions.
     * @param width the width of the decoder window
     * @param height the height of the decoder window
     * @param mult_ if 0 the decoder's dimension is set to 640x480
     */
    Decoder(int width, int height, bool mult_ = 0);
    ~Decoder();
    int framecounter;
    int *linesize;
    struct SwsContext* sws;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time, m_end_time;

public slots:
    /**
     * @brief process called when the decoder's thread has started so that the decoder will start its own initializations.
     */
    void process();
    /**
     * @brief decode reconstructs a YUV-image from the given encoded data packet
     * @param size the length of the packet
     * @param data the pointer to the data of the packet
     */
    void decode(int size, uchar* data);

signals:
    void finished();
    /**
     * @brief sendYuvFrame sends the image data to the yuvviewer along with the received sideinformation for fixed-size window and roi
     */
    void sendYuvFrame(uchar**, int*);
    void processFrame();
    void copyFrame(cv::Mat*);

private:
    AVFormatContext *m_pFormatCtx;
    AVCodecContext *m_pCodecCtxOrig;
    AVCodecContext *m_pCodecCtx;
    int m_videoStream;
    AVCodec *m_pCodec;
    AVFrame *m_pFrame;
    AVPicture pic_rgb;
    uint8_t *pixels;
    cv::Mat currentFrame;
    uint8_t *m_buffer;
    int m_frameFinished;
    AVPacket m_packet;
    int m_width;
    int m_height;
    bool mult;
    int m_iFrame;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time, end_time, stcc, etcc;

    int fd,RTS_flag,set_flag, prev_pix, diff;
    QFile *m_file;

};

#endif // DECODER_H
