#ifndef ENCODER_H
#define ENCODER_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <iostream>

#include <QObject>
#include <QUdpSocket>
#include <QFile>

extern "C" {
#include <x264.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
}
/**
 * @brief The Encoder class encodes the received image data into H264 Video Format. Images are converted to YUV-format before.
 * Then the stream is sent both to the integrated decoder partner and over the network
 */
class Encoder : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Encoder constructs an encoder instance
     * @param width the width of the images to be encoded
     * @param height the height of the images to be encoded
     * @param fps the framerate (i think its only used for stats calculation of the used bandwidth, those stats are wrong when using fixed-size windows)
     * @param mult_ if 0 the dimension of the encoder will be 640x480
     */
    Encoder(int width, int height, int fps, int tmax, bool mult_, QFile *file);
    ~Encoder();
    /**
     * @brief open opens the encoder
     * @return returns whether the encoder could be opened
     */
    bool open();
   private:
    bool validateSettings();                                      /**< validates if all params are set correctly, like width,height, etc.. */
    void setParams();                                             /**< sets the x264 params */
    int m_numBframes;
    int m_presetLength;
    char *m_preset;
    int m_qp;
    int frame_size;
    QFile *m_file;
    QUdpSocket *udp;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time,end_time;



  public slots:
    /**
     * @brief closeSlot closes the encoder end cleans up the resources
     */
    void closeSlot();
    /**
     * @brief encodeSlot encodes the given image data to H264 video format with image type YUV
     * @param pixels the pointer to the data of the image
     */
    void encodeSlot(uchar* pixels, int framenumber, int keyframe);
    /**
     * @brief process is called when encoder's thread has started to let encoder start to initialize
     */
    void process();

  signals:
    void finished();
    /**
     * @brief writeToNetwork sends encoded data along with sideinformation of fixed-size window and roi to networkstreamer class
     */
    void writeToNetwork(int, u_int8_t*);


   public:
    /* params the user should set */
    int m_width;
    int m_height;
    AVPixelFormat in_pixel_format,out_pixel_format;
    int m_fps,framecounter,m_tmax;                                                       /* e.g. 25, 60, etc.. */

    /* input / output */
    int pts;
    struct SwsContext* sws;
    FILE* fp;
    x264_t* encoder;
    int num_nals;


    AVOutputFormat *fmt;
    AVFormatContext *ctx;
    AVStream *stream;

    /* x264 */
    AVPicture pic_raw;                                            /* used for our "raw" input container */
    x264_picture_t pic_in;
    x264_picture_t pic_out;
    x264_param_t params;
    x264_nal_t* nals;


    int m_iFrame;
    uchar* m_pixels;

    bool mult;

    bool streamNetwork;

    std::string preset;
    std::string tune;
    std::string level;

};

#endif // ENCODER_H
