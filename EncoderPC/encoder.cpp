#include "encoder.h"
#include <iostream>

Encoder::Encoder(int width, int height, int fps, int tmax, bool mult_, QFile *file)
    :m_width(width)
    ,m_height(height)
    ,in_pixel_format(AV_PIX_FMT_GRAY8) //For color: AV_PIX_FMT_BGR24
    ,out_pixel_format(AV_PIX_FMT_YUV420P)
    ,m_fps(fps)
    ,fp(NULL)
    ,encoder(NULL)
    ,sws(NULL)
    ,num_nals(0)
    ,pts(0)
    ,m_numBframes(0)
    ,m_presetLength(0)
    ,m_preset("")
    ,m_qp(-1)
    ,frame_size(0)
    ,m_iFrame(1)
    ,m_pixels(NULL)
    ,preset("")
    ,tune("")
    ,level("")
    ,streamNetwork(true)
    ,mult(mult_)
    ,m_tmax(tmax)
    ,m_file(file)
{
}

Encoder::~Encoder()
{
    std::cout << "Encoder destructor called" << std::endl;
    if(sws)
    {
        closeSlot();
    }
    free(m_pixels);
}

void Encoder::process()
{
    this->open();

    int w = m_width;
    int h = m_height;


    m_pixels = (uchar*)malloc(w*h*3*sizeof(uchar));
    memset(m_pixels, 0, w*h*3);
    udp = new QUdpSocket();
}

bool Encoder::open() {

  if(!validateSettings()) {
    emit finished();
    return false;
  }

  int r = 0;
  int nheader = 0;
  int header_size = 0;
  framecounter=0;

  // @todo add validate which checks if all params are set (in/out width/height, fps,etc..);
  if(encoder) {
    std::cout << "Already opened. first call close()" << std::endl;
    emit finished();
    return false;
  }

  if(out_pixel_format != AV_PIX_FMT_YUV420P) {
    std::cout << "At this moment the output format must be AV_PIX_FMT_YUV420P" << std::endl;
    emit finished();
    return false;
  }

  sws = sws_getContext(m_width, m_height, in_pixel_format,
                       m_width, m_height, out_pixel_format,
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);

  if(!sws) {
    std::cout << "Cannot create SWS context" << std::endl;
    ::exit(EXIT_FAILURE);
  }


  x264_picture_alloc(&pic_in, X264_CSP_I420, m_width, m_height);


  setParams();


  // create the encoder using our params
  encoder = x264_encoder_open(&params);
  if(!encoder) {
    std::cout << "Cannot open the encoder" << std::endl;
    goto error;
  }

  // write headers
  r = x264_encoder_headers(encoder, &nals, &nheader);
  if(r < 0) {
    std::cout << "x264_encoder_headers() failed" << std::endl;
    goto error;
  }

  pts = 0;
  return true;


 error:
  closeSlot();
  emit finished();
  return false;
}

void Encoder::encodeSlot(uchar *pixels, int framenumber, int keyframe)
{

    if(!sws) {
      std::cout << "Not initialized, so cannot encode" << std::endl;
      emit finished();
      return;
    }

    // copy the pixels into our "raw input" container.
    int bytes_filled = avpicture_fill(&pic_raw, (uint8_t*)pixels, in_pixel_format, m_width, m_height);
    if(!bytes_filled) {
      std::cout << "Cannot fill the raw input buffer" << std::endl;
      emit finished();
      return;
    }


    // convert to I420 for x264
    int h = sws_scale(sws, pic_raw.data, pic_raw.linesize, 0,
                      m_height, pic_in.img.plane, pic_in.img.i_stride);
    if(h != m_height) {
      std::cout << "scale failed: %d" << std::endl;;
      emit finished();
      return;
    }


    // and encode and store into pic_out
    pic_in.i_pts = pts;
//    start_time = std::chrono::high_resolution_clock::now();
    frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
//    end_time = std::chrono::high_resolution_clock::now();
//    std::cout << "Framesize: " << frame_size << " bytes." << std::endl;

    if(frame_size)
    {
//        std::cout << "Successfully encoded, now calling writeToNetwork" << std::endl;
        char frametype = (char ) keyframe;
//        if(keyframe){printf("we have a keyframe!\n");}
//        printf("%#02x\n",frametype);
        QByteArray data(frametype,1);
//        QByteArray imagedata(reinterpret_cast<char*>(nals[0].p_payload), frame_size);
        data.append(reinterpret_cast<char*>(nals[0].p_payload), frame_size);
//        udp->writeDatagram(data, QHostAddress("127.0.0.1"), 4999);
        udp->writeDatagram(data, QHostAddress("10.152.4.124"), 5000);
//        std::cout << "Actual sent size: " << imagedata.size() << std::endl;
//        QTextStream stream(m_file);
//        stream << framenumber << "," << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << ","
//                  << frame_size << ","
//                  << frame_size/125
//                  << "\n";
//        emit writeToNetwork(frame_size, nals[0].p_payload);
//        if(framecounter == m_fps){
//        std::cout << std::endl << ", ENC_time (us): "
//                  << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()
//                  << ", Framesize (byte): "
//                  << frame_size
//                  << ", TRA_time(us): "
//                  << frame_size/125         //1Gbit/s = 1000bits/us = 125bytes/us
//                  << std::endl;
//        framecounter=0;
//        }
        framecounter++;
    }

    ++pts;

}


void Encoder::closeSlot() {
    std::cout << "ENCODER CLOSE SLOT CALLED" << std::endl;
  if(encoder) {
    x264_picture_clean(&pic_in);
    memset((char*)&pic_in, 0, sizeof(pic_in));
    memset((char*)&pic_out, 0, sizeof(pic_out));

    x264_encoder_close(encoder);
    encoder = NULL;
  }

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  memset((char*)&pic_raw, 0, sizeof(pic_raw));

  if(fp) {
    fclose(fp);
    fp = NULL;
  }
  emit finished();
}

void Encoder::setParams() {

  if(preset.empty())
  {
      x264_param_default_preset(&params, "ultrafast", "zerolatency,fastdecode");
      x264_param_apply_profile(&params, "baseline");


      if(m_numBframes == 0)
      {
          params.i_keyint_max = 1;              //only I frames <-> 0 frames delay
      }
      else
      {
          params.i_keyint_max = m_numBframes + 2;
          params.i_bframe = m_numBframes;
          params.i_bframe_pyramid = 0;
      }

      if(m_qp >= 0)
      {
          params.rc.i_qp_max = m_qp;
          params.rc.i_qp_min = m_qp;
      }

  }
  else
  {
      x264_param_default_preset(&params, preset.c_str(), tune.c_str());
      x264_param_apply_profile(&params, level.c_str());
  }

  params.i_threads = 1;
  params.i_width = m_width;
  params.i_height = m_height;
  params.i_fps_num = m_fps;
  params.i_fps_den = 1;
  m_fps=m_fps/(m_tmax+1);

  //Felix' Parameters

//  if(preset.empty())
//  {
//      x264_param_default_preset(&params, "ultrafast", "zerolatency,fastdecode");
//      x264_param_apply_profile(&params, "baseline");


//      if(m_numBframes == 0)
//      {
//          params.i_keyint_max = 1;              //only I frames <-> 0 frames delay
//      }
//      else
//      {
//          params.i_keyint_max = m_numBframes + 2;
//          params.i_bframe = m_numBframes;
//          params.i_bframe_pyramid = 0;
//      }

//      if(m_qp >= 0)
//      {
//          params.rc.i_qp_max = m_qp;
//          params.rc.i_qp_min = m_qp;
//      }

//  }
//  else
//  {
//      x264_param_default_preset(&params, "ultrafast", "zerolatency,fastdecode");
//      x264_param_apply_profile(&params, "baseline");
//  }

//  params.i_threads = 0; //Felix!
//  params.i_width = m_width;
//  params.i_height = m_height;
//  params.i_fps_num = m_fps;
//  params.i_fps_den = 1;
//  m_fps=m_fps/(m_tmax+1);

//  params.rc.i_qp_max = 20;
//  params.rc.i_qp_min = 20;

    /* x264_param_default_preset(&params, "ultrafast", "zerolatency,fastdecode");
    x264_param_apply_profile(&params, "baseline");

    params.i_threads = 0; //Felix!
    params.i_width = m_width;
    params.i_height = m_height;
    params.i_fps_num = m_fps;
    params.i_fps_den = 1;
    m_fps=m_fps/(m_tmax+1);*/

  //Ratecontrol Constant Quantizer
  //params.rc.i_rc_method = X264_RC_CQP;
  //params.rc.i_qp_constant = 23;
  //params.rc.i_qp_min = 23;
  //params.rc.i_qp_max = 23;
  //params.rc.i_qp_step = 0;
  //params.rc.i_aq_mode = X264_AQ_NONE;

  //Ratecontrol Constant Bitrate
  //params.rc.i_rc_method = X264_RC_ABR;
  //params.rc.i_bitrate = 1000;               //bitrate in kilobits per second
  //params.rc.f_rate_tolerance = 10;
  //params.b_intra_refresh = 1;

  //No Delayed Frames
  //params.rc.b_mb_tree = 0;                  //no marcoblock tree algorithm
  //params.i_bframe = 0;                      //no BFrames
  //params.i_sync_lookahead = 0;              //no Encoder Lookahead Frames
//    params.b_sliced_threads = 1;
  //params.b_vfr_input = 0;                   //no Variable Frame Rate

  //Fast Encoding
//  params.analyse.b_transform_8x8 = 0;                       //no 8x8 DCT
   //params.analyse.i_me_method = X264_ME_TESA;                 //use Diamond Search as Motion Estimation Algorithm
//  params.analyse.i_me_range = 16;                           //set search range for Motion Estimation Algorithm to 16 pixels
   //params.analyse.i_subpel_refine = 4;                       //fullpixel precision for Motion Estimation
//  params.analyse.i_direct_mv_pred = X264_DIRECT_PRED_NONE;  //no Prediction for direct motion vectors
//  params.i_frame_reference = 1;                             //use one reference frame for inter frame prediction
  //params.b_cabac = 1;                                       //use CAVLC instead of CABAC
//  params.b_deblocking_filter = 1;                           //use Deblocking Filter
//  params.analyse.intra = 0;                                 //no partitioning of macroblocks into prediction blocks for intra frame prediction
//  params.analyse.inter = 0;                                 //no partitioning of macroblocks into prediction blocks for inter frame prediction
//  params.analyse.f_psy_rd = 0;                              //no psychovisual rate distortion optimization
//  params.analyse.f_psy_trellis = 0;                         //no psychovisual trellis optimization
//  params.analyse.b_psy = 0;                                 //no psy optimizations
//  params.i_keyint_max = 250;                                //set GOP size to 250


}

bool Encoder::validateSettings() {
  if(!m_width) {
    std::cout << "No width set" << std::endl;
    emit finished();
    return false;
  }
  if(!m_height) {
    std::cout << "No height set" << std::endl;
    emit finished();
    return false;
  }
  if(in_pixel_format == AV_PIX_FMT_NONE) {
    std::cout << "No in_pixel_format set" << std::endl;
    emit finished();
    return false;
  }
  if(out_pixel_format == AV_PIX_FMT_NONE) {
    std::cout << "No out_pixel_format set" << std::endl;
    emit finished();
    return false;
  }
  return true;
}
