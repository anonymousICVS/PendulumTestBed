# Source code for the Encoder PC

This is the PC connected to the Camera, compresses the video and sends it to the [DecoderPC](https://github.com/cbachhuber/PendulumTestBed/tree/master/DecoderPC). 

The program is structured as follows: 

- in file [main.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/EncoderPC/main.cpp), parameters such as image resolution and frame rate are defined. The code in this file furthermore defines, connects and starts the two sub-threads.
- the first thread is [threadCam](https://github.com/cbachhuber/PendulumTestBed/blob/master/EncoderPC/main.cpp#L48), the camera frame grabber thread, defined in [cameraframegrabber.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/EncoderPC/cameraframegrabber.cpp). If grabs frames from the camera and forwards them to the encoder thread.
- The [encoder thread](https://github.com/cbachhuber/PendulumTestBed/blob/master/EncoderPC/main.cpp#L54), defined in [encoder.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/EncoderPC/encoder.cpp) compresses the video images from the camera thread using x264 and sends them over a network to the [DecoderPC](https://github.com/cbachhuber/PendulumTestBed/tree/master/DecoderPC).

To open the project, install the [required software](https://github.com/cbachhuber/PendulumTestBed/tree/master/DecoderPC) on your computer, in particular Qt, and open [CodingPendulum.pro](https://github.com/cbachhuber/PendulumTestBed/blob/master/EncoderPC/CodingPendulum.pro) in Qt. Compilation and program running can be done from within Qt.
