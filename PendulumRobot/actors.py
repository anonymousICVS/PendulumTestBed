
import pyudev
import time
import os
from math import *
from structs_and_constants import *

class EV3Motor:
    def __init__(self, which=0):
        devices = list(pyudev.Context().list_devices(subsystem='tacho-motor') \
                .match_attribute('driver_name', 'lego-ev3-l-motor'))

        if which >= len(devices):
            raise Exception("Motor not found")

        self.device = devices[which]
        self.pos = open(os.path.join(self.device.sys_path, 'position'), 'r+',0)
        self.duty_cycle_sp = open(os.path.join(self.device.sys_path,'duty_cycle_sp'), 'w', 0)

        self.reset()

    def __del__(self):
        self.pos.close()
        self.duty_cycle_sp.close()

    def reset(self):
        self.set_pos(0)
        self.set_duty_cycle_sp(0)
        self.send_command("run-direct")
    def get_pos(self):
        self.pos.seek(0)
        return int(self.pos.read())

    def set_pos(self, new_pos):
        return self.pos.write(str(new_pos))

    def set_duty_cycle_sp(self, new_duty_cycle_sp):
        return self.duty_cycle_sp.write(str(new_duty_cycle_sp))

    def send_command(self, new_mode):
        with open(os.path.join(self.device.sys_path, 'command'),'w') as command:
            command.write(str(new_mode))

class EV3Motors:
    def __init__(self, left=0, right=1):
        self.left = EV3Motor(left)
        self.right = EV3Motor(right)
        self.pos = 0.0
        self.speed = 0.0
        self.diff = 0
        self.target_diff = 0
        self.steer = 0
        self.prev_sum = 0
        self.r = 0.04125
        self.prev_deltas = [0,0,0]

    def get_data(self, interval, theta = 0.):
        left_pos = self.left.get_pos()
        right_pos = self.right.get_pos()

        pos_sum = ((right_pos + left_pos)/2.  * pi * (self.r) / (2. * 180.)) + theta * self.r
        self.diff = left_pos - right_pos

        delta = pos_sum - self.prev_sum
        self.pos += delta

        self.speed = (delta + sum(self.prev_deltas)) / (4 * interval)

        self.prev_sum = pos_sum
        self.prev_deltas = [delta] + self.prev_deltas[0:2]

        return self.speed, self.pos

    def reset(self):
        self.left.reset()
        self.right.reset()
        
    def steer_control(self, power, steering, interval):
        power_left = max(-100, min(power , 100))
        power_right = max(-100, min(power, 100))
        return power_left, power_right

def set_actors(motors, values):
    M1_duty_cycle_sp = values[H2R_PACKET_VARS.M1_duty_cycle_sp]
    M2_duty_cycle_sp = values[H2R_PACKET_VARS.M2_duty_cycle_sp]
    motors.left.set_duty_cycle_sp(M1_duty_cycle_sp)
    motors.right.set_duty_cycle_sp(M2_duty_cycle_sp)


