# Source code for the Pendulum Robot

This is the code for the Pendulum connected to the [DecoderPC](https://github.com/cbachhuber/PendulumTestBed/tree/master/DecoderPC) through the wifi router. It uses the angle of inclination to keep itself upright.

The program is structured as follows: The file [controller.py](https://github.com/cbachhuber/PendulumTestBed/blob/master/PendulumRobot/controller.py) is the main file, all others are supplemental files for access to sensors and actors. File controller.py contains a main loop which in each iteration gathers sensor data such as motor positions and battery, receives the angle and angle speed from the DecoderPC, applies our controller to the state values, and sends the corresponding motor commands.

All the code in this folder just needs to be pushed to the pendulum, and controller.py to be executed. The ev3dev OS supports all the APIs used here, not additional packages are required.
