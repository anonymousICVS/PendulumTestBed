#include "decoder.h"

Decoder::Decoder(int width, int height, bool mult_) //0->directDecode; 1->networkStream
    :m_pFormatCtx(NULL),
      m_pCodecCtxOrig(NULL),
      m_pCodecCtx(NULL),
      m_videoStream(-1),
      m_pCodec(NULL),
      m_pFrame(NULL),
      m_frameFinished(0),
      m_iFrame(1),
      mult(mult_),
      m_width(width),
      m_height(height),
      prev_pix(255)
{
    av_register_all();
    avformat_network_init();
}

Decoder::~Decoder()
{
//    std::cout << "DECODER DESTRUCTOR CALLED" << std::endl;

    // Free the YUV frame
    av_free(m_pFrame);

    // Close the codecs
    avcodec_close(m_pCodecCtx);
    avcodec_close(m_pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&m_pFormatCtx);
    m_file->close();
}

void Decoder::process()
{
    av_init_packet(&m_packet);

    m_pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    avcodec_alloc_context3(m_pCodec);
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);

    m_pCodecCtx->width = (m_width);
    m_pCodecCtx->height = (m_height);
    m_pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    //Felix
    m_pCodecCtx->thread_count = 4;
    m_pCodecCtx->thread_type = FF_THREAD_SLICE;

    // Open codec
    if(avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0)
    {
        fprintf(stderr, "avcodec_open2 error !\n");
        emit finished();
    }

    // Allocate video frame
    m_pFrame = av_frame_alloc();

    //Serial Setup
//    fd = open("/dev/ttyS0",O_RDWR | O_NOCTTY );
//    if(fd == -1)
//        {printf("Error in opening ttyS0\n");}
//    else
//        {printf("ttyS0 Opened Successfully \n");}
//    RTS_flag = TIOCM_RTS;

//    Open log file
    m_file = new QFile("log_dec.csv");
    if(!(m_file->open(QFile::WriteOnly))){
        std::cout << "Error opening the log file!" << std::endl;
    }
    //Setting up the color space conversion
    sws = sws_getContext(m_width, m_height, AV_PIX_FMT_YUV420P,
                         m_width, m_height, AV_PIX_FMT_GRAY8,
                         SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if(!sws) {
      std::cout << "Cannot create SWS context" << std::endl;
      ::exit(EXIT_FAILURE);
    }

    //Allocate the RGB picture
    pixels = (uint8_t *)malloc(m_width*m_height*3);
    currentFrame.create(m_height, m_width, CV_8UC1);
}


void Decoder::decode(int size, uchar* data)
{
//    std::cout << "Framesize: " << size << " bytes." << std::endl;

    m_packet.size = size;
    m_packet.data = data;

    start_time = std::chrono::high_resolution_clock::now();
    avcodec_decode_video2(m_pCodecCtx, m_pFrame, &m_frameFinished, &m_packet);
    end_time = std::chrono::high_resolution_clock::now();
    // Did we get a video frame?
    if(m_frameFinished)
    {
//        std::cout << "Finished decoding!" << std::endl;
//        *(m_pFrame->linesize) = *(m_pFrame->linesize) * 3;
//        std::cout << m_pFrame->linesize[1] << std::endl;

        //Color conversion to Gray8, saved in cv::UMAT
        int bytes_filled = avpicture_fill(&pic_rgb, pixels, AV_PIX_FMT_GRAY8, m_width, m_height);
        if(!bytes_filled) {
          std::cout << "Cannot fill the raw input buffer" << std::endl;
          emit finished();
          return;
        }

        // convert to GRAY8 for opencv
        stcc = std::chrono::high_resolution_clock::now();
        int h = sws_scale(sws, m_pFrame->data, m_pFrame->linesize, 0,
                          m_height, pic_rgb.data, pic_rgb.linesize);
        etcc = std::chrono::high_resolution_clock::now();
        if(h != m_height) {
          std::cout << "scale failed: %d" << std::endl;
          emit finished();
          return;
        }

        //Putting the pixel data into a Mat
        currentFrame.data = *pic_rgb.data;
        emit copyFrame(&currentFrame);
        emit processFrame();
//        std::cout << "Test" << std::endl;

        //For display
        emit sendYuvFrame(m_pFrame->data, m_pFrame->linesize);
//        printf("%d\n", **(m_pFrame->data));

        //Serial Communication
//        diff=**(m_pFrame->data) - prev_pix;
//        prev_pix = **(m_pFrame->data);
//        printf("%d\n",diff);
//        if(diff > 15){
//            set_flag = ioctl(fd, TIOCMBIS, &RTS_flag);
//    //        printf("ioctl returned %d.\n",set_flag);
//        }else{
//            ioctl(fd, TIOCMBIC, &RTS_flag);
//        }
        QTextStream stream(m_file);
        stream << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << ", "
               <<  std::chrono::duration_cast<std::chrono::microseconds>(etcc - stcc).count() << "\n";
//            printf("%d, %d, %d, %d\n",*(m_pFrame->data[0]),*(m_pFrame->data[1]), *(m_pFrame->data[2]), *(m_pFrame->data[3]));

    }
//    std::cout << "Decoding time (us): "
//              << std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count()
//              << std::endl;

    av_free_packet(&m_packet);
}
