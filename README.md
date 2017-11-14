# A Testbed for Vision-Based Networked Control Systems
This repository contains instructions and source code to set up your very own testbed as presented in paper "A Testbed for Vision-Based Networked Control Systems" ([DOI](http://dx.doi.org/10.1007/978-3-319-68345-4_3)). The testbed balances an inverted pendulum in a vertical position, visually controlled by a camera.

It is structured as follows:  folder 'EncoderPC' contains the C++ source for the PC which is connected to the camera that records the video of the pendulum. 'EncoderPC' encodes that video and sends it to 'DecoderPC'. Folder 'DecoderPC' contains the C++ source code for the PC which decodes the video and applies image processing to it and sends the angle to the 'PendulumRobot'. Folder 'PendulumRobot' contains the code and images of the inverted pendulum.
 
### Hardware Prerequisites:
- 2 Desktop computers with Ubuntu 16.04 LTS and a C++ compiler (ideally use Qt, it's free for open source projects)
- [LEGO Mindstorms EV3 Kit](https://www.amazon.com/LEGO-6029291-Mindstorms-EV3-31313/dp/B00CWER3XY/ref=sr_1_1?ie=UTF8&qid=1491830776&sr=8-1) + [Big wheels](https://www.bricklink.com/v2/catalog/catalogitem.page?P=2903c02#T=C&C=1)
- WiFi USB Dongle (e.g. [EDIMAX EW-7811UN](https://www.amazon.com/Edimax-EW-7811Un-150Mbps-Raspberry-Supports/dp/B003MTTJOY/ref=sr_1_1?ie=UTF8&qid=1491830898&sr=8-1))
- WiFi router (e.g. TP-Link TD-W8970B)
- USB Camera (e.g. [XIMEA MQ022-CG-CM](https://www.ximea.com/en/products/cameras-filtered-by-sensor-types/mq022mg-cm) + [suitable lens](https://www.baslerweb.com/en/products/vision-components/lenses/ricoh-lens-fl-cc0614a-2m-f1-4-f6mm-2-3/) + tripod)
- Optional, but recommended: the Mindstorms EV3 [rechargeable battery](https://www.amazon.com/LEGO-Mindstorms-EV3-Rechargeable-Battery/dp/B00G1IMOEA/ref=sr_1_2?ie=UTF8&qid=1491830834&sr=8-2) and [charger](https://www.amazon.com/LEGO-Mindstorms-9833-Transformer-Charger/dp/B003BCLOAY/ref=sr_1_3?ie=UTF8&qid=1491830834&sr=8-3)

### Software Setup
Set up the computers
- On the EncoderPC, install OpenCV, x264, and the [XIMEA linux driver](https://www.ximea.com/support/wiki/apis/XIMEA_Linux_Software_Package). Copy the files from EncoderPC folder to a new directory and open the Qt project
- On the DecoderPC, install OpenCV, SDL2, and ffmpeg. Copy the files from the DecoderPC to a new directory and open the Qt project. Make sure that on both PCs, the projects compile.

Set up the EV3 robot
- Install [ev3dev](http://www.ev3dev.org/docs/getting-started/)
- [Set up a WiFi-Connection](http://www.ev3dev.org/docs/networking/) to the DecoderPC
- Copy the python files from folder PendulumRobot into the robot's home directory, do not copy folder PendulumRobot/images

### Physical Setup
Build the robot from the Lego parts, according to [these images](PendulumRobot/images/). Glue the [visual markers](visualMarkers.pdf) to the robot as seen in [this image](PendulumRobot/images/left.jpg). Put the Wifi dongle into the [USB Port](PendulumRobot/images/right.jpg).

The information flow chain is as follows: Camera -> EncoderPC -> DecoderPC -> WiFi Router -> PendulumRobot. Connect the elements to each other, assign ip addresses and ports accordingly in the code ([EncoderPC](EncoderPC/encoder.cpp#L165), [DecoderPC](DecoderPC/constants.h#L27), [PendulumRobot](PendulumRobot/controller.py#L84)).

Align the angle of the camera horizontally and put the robot in the center of the image such that both visual markers are visible. Find out the angle the DecoderPC reports when manually bringing the pendulum to the balancing point (after running the projects on the encoderPC and the DecoderPC). Subtract this value in [controller.py, line 206](PendulumRobot/controller.py#L206). Be careful with the light: tune the camera aperture such that the DecoderPC reports a 'Light' value between 90 and 110. Ideally, use a DC light source.

### Running the testbed
1. Run the Qt project on the EncoderPC
2. Run the Qt project on the DecoderPC
3. Ssh to the robot (user: robot, pwd: maker) and run the python code 'controller.py' while manually holding the pendulum in a vertical position
4. After a short initialization period, the pendulum will start balancing!

### Troubleshooting
The pendulum is not balancing? Make sure that
- EncoderPC sends data (check the System Monitor)
- DecoderPC receives data
- PendulumRobot receives data
- PendulumRobot is in the field of view of the camera


### Questions?
If you need any help, don't hesitate to contact me: christoph.bachhuber@tum.de
