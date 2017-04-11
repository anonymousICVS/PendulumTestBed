#include "cameraframegrabber.h"


cameraFrameGrabber::cameraFrameGrabber(shutdownHandler *shH, int x, int y, int xoffset, int yoffset, int exposure, bool autoExposureGain, int imgFormat, int tmin, int tmax, double thr, int fps, QFile *file)
    :m_capture(NULL),
      m_first(true),
      m_shH(shH),
      m_x(x),
      m_y(y),
      m_xoffset(xoffset),
      m_yoffset(yoffset),
      m_fps(fps),
      m_exposure(exposure),
      m_autoExposureGain(autoExposureGain),
      m_imgFormat(imgFormat),
      m_thr(thr),
      m_tmin(tmin),
      m_tmax(tmax),
      m_file(file)
{    
}

cameraFrameGrabber::~cameraFrameGrabber()
{
    std::cout << "cameraframgrabber destructor called" << endl;
    m_capture->release();
    emit down();
}

void cameraFrameGrabber::process()
{
    m_tmin = min(m_tmin,m_tmax);
    m_capture = new VideoCapture( CV_CAP_XIAPI );
    if ( !m_capture ) {
        fprintf( stderr, "ERROR: capture is NULL \n" );
        emit finished();
    }
    else
    {
        m_capture->set(CV_CAP_PROP_XI_AEAG, (int)m_autoExposureGain);
        m_capture->set(CV_CAP_PROP_EXPOSURE, m_exposure);               //5882 is 170 Hz... //10000 is 100 Hz and avoids flickering //5000 is 200 Hz //2000 is 500 Hz
        m_capture->set(CV_CAP_PROP_XI_DATA_FORMAT, m_imgFormat);        //XI_RAW8 -> 5 (1 channel, 0...255, quickest mode); //XI_RGB24 -> 2 (3 channels, 0...255, standard mode, 3x more load on usb-bus)
        m_capture->set(CV_CAP_PROP_FRAME_WIDTH,m_x);
        m_capture->set(CV_CAP_PROP_FRAME_HEIGHT,m_y);
        m_capture->set(CV_CAP_PROP_XI_OFFSET_X, m_xoffset);
        m_capture->set(CV_CAP_PROP_XI_OFFSET_Y, m_yoffset);
        m_capture->set(CV_CAP_PROP_XI_AUTO_WB,0);

        Mat testFrame, prevFrame;

        //Set up writing to the log file
        QTextStream stream(m_file);

        //read first frame to find out height and width
        m_capture->read(testFrame);

        MatSize sz = testFrame.size;

        //set up some image containers according to found size
        m_currentFrame.create(sz[0], sz[1], CV_8UC1);
        prevFrame.create(sz[0], sz[1], CV_8UC1);
        prevFrame = testFrame.clone();
        //        cout << sz[0] << sizeof(CV_8UC1) << endl;

        std::cout << "Exposure:" << m_capture->get(CV_CAP_PROP_EXPOSURE) << std::endl;


        auto start_time = std::chrono::high_resolution_clock::now();
        int frames_cam=0,t_since_last_keyframe=0,t_since_last_baseframe=0, framecounter=0, overall_frames=0;
        Scalar adiff;
        double meanabsdiff_channels=0;
        uchar * pointerbuffer;

        while(1 && (m_shH->m_shutdown == false))
        {
            frames_cam++;
            overall_frames++;
            t_since_last_keyframe++;
            t_since_last_baseframe++;

            //read next frame from camera
            m_capture->read(m_currentFrame);

            //if empty -> restart the loop
            if ( m_currentFrame.empty() ) {
                fprintf( stderr, "ERROR: frame is null...\n" );
                emit finished();
                break;
            }


            emit processFrame((uchar *)m_currentFrame.data, overall_frames, 1); 
            framecounter++;
            auto end_time = std::chrono::high_resolution_clock::now();

            stream << overall_frames << "," << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_fs).count() << "," << meanabsdiff_channels << "\n";

            if(frames_cam == m_fps){
                std::cout << "\r"
                          << "fps_Cam (Hz): "
                          << (double)frames_cam*1000/(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count())
                          << std::flush;
                start_time = std::chrono::high_resolution_clock::now();
                frames_cam = 0;
                framecounter =0;
            }
        }
        m_file->close();
        emit closeEncoder();
        emit finished();
    }


}
