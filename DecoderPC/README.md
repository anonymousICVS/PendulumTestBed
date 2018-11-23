# Source code for the Decoder and Image Processing PC

This is the PC connected to [EncoderPC](https://github.com/cbachhuber/PendulumTestBed/tree/master/EncoderPC) and the wifi router.  It decodes the compressed video, computes the Pendulum's angle of inclination, and sends the angle and angle velocity to the [PendulumRobot](https://github.com/cbachhuber/PendulumTestBed/tree/master/PendulumRobot).

The program is structured as follows: 

- The file [main.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/DecoderPC/main.cpp) defines, connects and starts the four sub-threads.
- The first thread is [threadDec](https://github.com/cbachhuber/PendulumTestBed/blob/master/DecoderPC/main.cpp#L32), the decoder thread, defined in [decoder.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/DecoderPC/decoder.cpp). It decodes compressed frames from the EncoderPC and forwards them to color conversion thread
- The second important thread is the image processing thread, defined in [processing.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/DecoderPC/processing.cpp). It computes the Pendulum's angle of inclination by locating the visual marker in the image, and computing their angle relative to the vertical axis.
- The third thread sends this information to the pendulum. It is defined in [botcommunication.cpp](https://github.com/cbachhuber/PendulumTestBed/blob/master/DecoderPC/botcommunication.cpp).

To open the project, install the [required software](https://github.com/cbachhuber/PendulumTestBed/tree/master/DecoderPC) on your computer, in particular Qt, and open [CodingPendulumReceiver.pro](https://github.com/cbachhuber/PendulumTestBed/blob/master/DecoderPC/CodingPendulumReceiver.pro) in Qt. Compilation and program running can be done from within Qt.
