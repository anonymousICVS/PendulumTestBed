## A Testbed for Networked Visually Controlled Systems
This repository contains instructions and source code to set up your very own testbed as presented in paper "A Testbed for Networked Visually Controlled Systems". It is structured as follows:  folder 'EncoderPC' contains the C++ source for the PC which is connected to the camera that records the video of the pendulum. 'EncoderPC' encodes that video and sends it to 'DecoderAndImageProcessingPC'. Folder 'DecoderAndImageProcessingPC' contains the C++ source code for the PC which decodes the video and applies image processing to it and sends the angle to the 'PendulumRobot'.
 
### Prerequisites:

You need the following hardware:
- 2 Desktop computers with Ubuntu 16.04 LTS and a C++ compiler (ideally use Qt)
- [LEGO Mindstorms EV3 Kit](https://www.amazon.com/LEGO-6029291-Mindstorms-EV3-31313/dp/B00CWER3XY/ref=sr_1_1?ie=UTF8&qid=1491830776&sr=8-1) + [Big wheels](https://www.bricklink.com/v2/catalog/catalogitem.page?P=2903c02#T=C&C=1)
- Optional, but recommended: the Mindstorms EV3 [rechargeable battery](https://www.amazon.com/LEGO-Mindstorms-EV3-Rechargeable-Battery/dp/B00G1IMOEA/ref=sr_1_2?ie=UTF8&qid=1491830834&sr=8-2) and [charger](https://www.amazon.com/LEGO-Mindstorms-9833-Transformer-Charger/dp/B003BCLOAY/ref=sr_1_3?ie=UTF8&qid=1491830834&sr=8-3)
- WiFi USB Dongle (e.g. [EDIMAX EW-7811UN](https://www.amazon.com/Edimax-EW-7811Un-150Mbps-Raspberry-Supports/dp/B003MTTJOY/ref=sr_1_1?ie=UTF8&qid=1491830898&sr=8-1))
- WiFi router (e.g. TP-Link TD-W8970B)
- USB Camera (e.g. [XIMEA MQ022-CG-CM](https://www.ximea.com/en/products/cameras-filtered-by-sensor-types/mq022mg-cm) + [suitable lens](https://www.baslerweb.com/en/products/vision-components/lenses/ricoh-lens-fl-cc0614a-2m-f1-4-f6mm-2-3/) + tripod)

### Physical Setup
Build the robot from the Lego parts, according to [these images](PendulumRobot/images/). Glue the [visual markers](visualMarkers.pdf) to the robot as seen in the images.

The information flow chain is as follows:Camera -> EncoderPC -> DecoderAndImageProcessingPC -> PendulumRobot. Connect the elements to each other

Align angle of camera horizontally and put the robot in the center of the image such that both visual markers are visible. Find out the angle the DecoderPC reports when manually bringing the pendulum to the balancing point. Subtract this value in [controller.py, line 206](PendulumRobot/controller.py#L206)

### Software Setup

Set up the computers:

Set up the EV3 robot:
- Install [ev3dev](http://www.ev3dev.org/)

### Questions?
If you need any help, don't hesitate to contact me: cbachhuber89@gmail.com
