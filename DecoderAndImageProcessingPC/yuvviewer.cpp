#include "yuvviewer.h"
#include <iostream>

using namespace std;

YuvViewer::YuvViewer(int width, int height, bool mult_, bool roi_copy_)
    :W(width)
    ,H(height)
    ,m_pWindow(NULL)
    ,m_pRenderer(NULL)
    ,m_pTexture(NULL)
    ,m_pSwsCtxYuv2Bgra(NULL)
    ,m_pSwsCtxYuv2Rgb_640(NULL)
    ,m_pSwsCtxRgb2Bgra(NULL)
    ,argb_src(NULL)
    ,argb_stride(NULL)
    ,m_event(NULL)
    ,mult(mult_)
    ,roi_copy(roi_copy_)
{
}

YuvViewer::~YuvViewer()
{
    cout << "rgbviewer deleted" << endl;
    SDL_DestroyTexture(m_pTexture);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
    sws_freeContext(m_pSwsCtxYuv2Bgra);

    free(argb_src);
    free(argb_stride);
    m_file->close();

}

void YuvViewer::process()
{
//    std::cout << "Initializing SDL" << std::endl;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    m_event = new SDL_Event();

    m_pWindow = SDL_CreateWindow("YuvViewer",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 2*W,
                                 2*H,
                                 SDL_WINDOW_OPENGL);
    if(!m_pWindow) {
        cout << "SDL: could not create window - exiting" << endl;
        emit finished();
    }

    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

    if(!m_pRenderer) {
        cout << "SDL: could not create renderer -exiting" << endl;
        emit finished();
    }

    m_pTexture = SDL_CreateTexture(m_pRenderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   W,
                                   H);
    if(!m_pTexture) {
        cout << "SDL: could not create texture -exiting" << endl;
        emit finished();
    }
//    SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN);
//    SDL_GL_SetSwapInterval(0); //Disabling vertical sync


    m_pSwsCtxYuv2Bgra = sws_getContext(W, H, AV_PIX_FMT_YUV420P, W, H, AV_PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL, NULL);
    m_pSwsCtxBgr2Bgra_120 = sws_getContext(W, H, AV_PIX_FMT_BGR24, W, H, AV_PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL, NULL);
//    m_pSwsCtxRgb2Bgra = sws_getContext(640, 480, AV_PIX_FMT_RGB24, 640, 480, AV_PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL, NULL);
//    m_pSwsCtxYuv2Rgb_640 = sws_getContext(640, 480, AV_PIX_FMT_YUV420P, 640, 480, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

//    patch_640.create(480, 640, CV_8UC3);
//    frame.create(480, 640, CV_8UC3);

    ls = (int*)malloc(sizeof(int)*1);
    ls[0] = W*3;



    uint8_t *argb_data = (uint8_t *)malloc(W*H*4*sizeof(uint8_t));
    argb_src = (uint8_t **)malloc(sizeof(uint8_t *)*4);
    argb_src[0] = argb_data;
    argb_src[1] = argb_data+W*H;
    argb_src[2] = argb_data+W*H*2;
    argb_src[3] = argb_data+W*H*3;
    argb_stride = (int *)malloc(sizeof(int)*1);
    argb_stride[0] = 4*W;

//    W = 640;
//    H = 480;

//    uint8_t *rgb_data_640 = (uint8_t *)malloc(W*H*3*sizeof(uint8_t));
//    rgb_src_640 = (uint8_t **)malloc(sizeof(uint8_t *)*3);
//    rgb_src_640[0] = rgb_data_640;
//    rgb_src_640[1] = rgb_data_640+W*H;
//    rgb_src_640[2] = rgb_data_640+W*H*2;
//    rgb_stride_640 = (int *)malloc(sizeof(int)*1);
//    rgb_stride_640[0] = 3*W;

    //Serial Setup
//    fd = open("/dev/ttyS0",O_RDWR | O_NOCTTY );
//    if(fd == -1)
//        {printf("Error in opening ttyS0\n");}
//    else
//        {printf("ttyS0 Opened Successfully \n");}
//    RTS_flag = TIOCM_RTS;


    //Open log file
//    m_file = new QFile("log_colorconv.csv");
//    if(!(m_file->open(QFile::WriteOnly))){
//        std::cout << "Error opening the log file!" << std::endl;
//    }
}

void YuvViewer::displayImage(uchar **data, int *linesize)
//void YuvViewer::displayImage(uchar *data, int *linesize)
{
//    SDL_UnlockTexture(m_pTexture);



    int test[1];
    test[0] = W*4;

    start_time_dis = std::chrono::high_resolution_clock::now();
    SDL_LockTexture(m_pTexture, NULL, reinterpret_cast<void**>(argb_src), test);
    start_time_cc = std::chrono::high_resolution_clock::now();
    sws_scale(m_pSwsCtxYuv2Bgra, data, linesize, 0, H, argb_src, argb_stride);
    end_time_cc = std::chrono::high_resolution_clock::now();
    SDL_UnlockTexture(m_pTexture);
    SDL_RenderClear(m_pRenderer);
    SDL_RenderCopy(m_pRenderer, m_pTexture, NULL, NULL);
    SDL_RenderPresent(m_pRenderer);
    end_time_dis = std::chrono::high_resolution_clock::now();

    //    printf("%d, %d, %d, %d\n",*(argb_src[0]),*(argb_src[1]), *(argb_src[2]), *(argb_src[3]));

    //Serial Communication
//    if(*(argb_src[0]) > 100){
//        set_flag = ioctl(fd, TIOCMBIS, &RTS_flag);
//        printf("ioctl returned %d.\n",set_flag);
//    }else{
//        ioctl(fd, TIOCMBIC, &RTS_flag);
//    }
//    QTextStream stream(m_file);
//    stream << std::chrono::duration_cast<std::chrono::microseconds>(end_time_cc-start_time_cc).count() << ","
//           << std::chrono::duration_cast<std::chrono::microseconds>(end_time_dis-start_time_dis).count() << "\n";


    SDL_PollEvent(m_event);
    switch(m_event->type) {
    case SDL_KEYDOWN:
        emit finished();
        return;
        break;
    default:
        break;
    }
}

