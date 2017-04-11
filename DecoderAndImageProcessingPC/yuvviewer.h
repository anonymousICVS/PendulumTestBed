#ifndef YUVVIEWER_H
#define YUVVIEWER_H

#include <QFile>
#include <QTextStream>
#include <QObject>
#include <stdio.h>
#include <fcntl.h>   	 /* File Control Definitions           */
#include <termios.h>	 /* POSIX Terminal Control Definitions */
#include <unistd.h> 	 /* UNIX Standard Definitions 	       */
#include <errno.h>   	 /* ERROR Number Definitions           */
#include <sys/ioctl.h>   /* ioctl()                            */
#include <chrono>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

//#include "core.hpp"

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
}


/**
 * @brief The YuvViewer class displays an image of type YUV in a SDL-Window
 */
class YuvViewer : public QObject
{

    Q_OBJECT

public:
    /**
     * @brief YuvViewer constructs the instance
     * @param mult_ tells yuvviewer whether fixed-size windows are used or not
     * @param roi_copy_ tells yuvviewer whether roi has to be cut out of current frame and stitched into previous frame or not
     */
    YuvViewer(int width, int height, bool mult_, bool roi_copy_);
    ~YuvViewer();

public slots:
    /**
     * @brief process called by yuvviewers thread when started to let yuvviewer do some initialization stuff
     */
    void process();
    /**
     * @brief displayImage will display the received image or image patch in a SDL-Window.
     * @brief data the pointer to the yuv image planes
     * @param linesize the linesize of the yuv image
     */
    void displayImage(uchar **data, int *linesize);
//    void displayImage(uchar *data, int *linesize);

signals:
    /**
     * @brief sendRgbFrame possibility to show the image also as RGB image in rgbviewer
     * @param data the image converted in rgb data
     */
    void sendRgbFrame(uchar *data);
    void finished();

private:
    int W, H;
    SDL_Window *m_pWindow;
    SDL_Renderer *m_pRenderer;
    SDL_Texture *m_pTexture;
    SwsContext *m_pSwsCtxYuv2Bgra;
    SwsContext *m_pSwsCtxYuv2Rgb_640;
    SwsContext *m_pSwsCtxBgr2Bgra_120;
    SwsContext *m_pSwsCtxRgb2Bgra;
    uint8_t **argb_src;
    uint8_t **yuv_src;
    int *argb_stride;
    uint8_t **rgb_src_640;
    uint8_t **rgb_src_160;
    int *rgb_stride_640;
    SDL_Event *m_event;
    AVPicture pic_raw;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_dis, start_time_cc, end_time_cc, end_time_dis;

    int *ls;

    int fd,RTS_flag,set_flag;
    QFile *m_file;


    bool mult;
    bool roi_copy;
};

#endif // YUVVIEWER_H
