

import struct
import time
import pyudev
import os
from math import *
import logging

EMAOFFSET = 0.0005

class Leds:
    def __init__(self):
        led_devices = list(pyudev.Context().list_devices(subsystem='leds'))
        self.current_red = 0
        self.left_red_path = led_devices[0].sys_path
        self.left_green_path = led_devices[1].sys_path
        self.right_red_path = led_devices[2].sys_path
        self.right_green_path = led_devices[3].sys_path

        self.left_red_f = open(os.path.join(self.left_red_path, 'brightness'), 'w')
        self.left_green_f = open(os.path.join(self.left_green_path, 'brightness'), 'w')
        self.right_red_f = open(os.path.join(self.right_red_path, 'brightness'), 'w')
        self.right_green_f = open(os.path.join(self.right_green_path, 'brightness'), 'w')
        
        self.write_left_green(0)
        self.write_left_red(0)
        self.write_right_green(0)
        self.write_right_red(0)

    def __del__(self):
        self.write_left_green(0)
        self.write_left_red(0)
        self.write_right_green(0)
        self.write_right_red(0)

        try:
            self.right_red_f.close()
            self.right_green_f.close()
            self.left_red_f.close()
            self.left_green_f.close()
        except Exception:
            print('Error closing files')
            pass

    def write_left_green(self, value):
        # with open(os.path.join(self.left_red_path, 'brightness'), 'w') as f:
        self.left_red_f.seek(0)
        self.left_red_f.write(str(value))

    def write_left_red(self, value):
        # with open(os.path.join(self.left_green_path, 'brightness'), 'w') as f:
        self.left_green_f.seek(0)
        self.left_green_f.write(str(value))

    def write_right_green(self, value):
        # with open(os.path.join(self.right_red_path, 'brightness'), 'w') as f:
        self.right_red_f.seek(0)
        self.right_red_f.write(str(value))

    def write_right_red(self, value):
        # with open(os.path.join(self.right_green_path, 'brightness'), 'w') as f:
        self.right_green_f.seek(0)
        self.right_green_f.write(str(value))

    def toggle_red(self):
        if self.current_red == 0:
            value = 100
            self.write_left_red(value)
            self.write_right_red(value)
            self.current_red = value
        elif self.current_red != 0:
            value = 0
            self.write_left_red(value)
            self.write_right_red(value)
            self.current_red = value


class Color:
    def __init__(self):
        devices = list(pyudev.Context().list_devices(subsystem='lego-sensor').match_attribute('driver_name', 'lego-ev3-color'))
        if not devices:
            raise Exception('Color Sensor not found')
        self.device_path = devices[0].sys_path
        self.current_mode = 'COL-REFLECT'
        self.col_f = open(os.path.join(self.device_path, 'mode'), 'w')
        self.set_mode(self.current_mode)

    def __del__(self):
        try:
            self.col_f.close()
        except Exception:
            print('err closing file')
            pass
        
    def set_mode(self, value):
        # with open(os.path.join(self.device_path, 'mode'), 'w') as f:
        self.col_f.seek(0)
        self.col_f.write(str(value))

    def flip_mode(self):
        if self.current_mode == 'COL-REFLECT':
            self.current_mode = 'COL-AMBIENT'
            self.set_mode(self.current_mode)
        elif self.current_mode == 'COL-AMBIENT':
            self.current_mode = 'COL-REFLECT'
            self.set_mode(self.current_mode)

class Gyro:
    @staticmethod
    def get_gyro():
        devices = list(pyudev.Context().list_devices(subsystem='lego-sensor').match_attribute('driver_name', 'lego-ev3-gyro'))
        if not devices:
            raise Exception('Gyro not found')
        return EV3Gyro(devices[0])

    def __init__(self, device):
        self.device = device
        self.value0 = open(os.path.join(self.device.sys_path, 'value0'), 'r', 0)
        self.value1 = None
        try:
            self.value1 = open(os.path.join(self.device.sys_path, 'value1'), 'r', 0)
        except:
            raise Exception('Not in correct mode to read value 1 speed')
        self.offset = 0.0
        self.angle = 0.0

    def __del__(self):
        self.value0.close()
        if self.value1 != None:
            self.value1.close()

    def calibrate(self):
        logging.debug('Calibrating gyro...')
        self.angle = 0.0
        total = 0
        for i in range(10):
            total += self.get_rate()
            time.sleep(0.01)
        average = total / 10.0
        self.offset = average - 0.1
        logging.debug('Done. offset: {}.'.format(self.offset))

    def set_mode(self, value):
        with open(os.path.join(self.device.sys_path, 'mode'), 'w') as f:
            f.write(str(value))

    def get_rate(self):
        if self.value1 == None:
            self.value0.seek(0)
            return int(self.value0.read())
        else:
            self.value1.seek(0)
            return int(self.value1.read())

    def get_angle(self):
        if self.value1 != None:
            self.value0.seek(0)
            return int(self.value0.read())

class EV3Gyro(Gyro):
    TYPE_ID = '32'
    def __init__(self, device):
        Gyro.__init__(self, device)
        print("Internal Calibration")
        self.set_mode("GYRO-CAL")
        time.sleep(1.)
        # self.set_mode("GYRO-RATE")
        self.set_mode("GYRO-G&A")

    def get_data(self, interval):
        gyro_raw = self.get_rate()
        self.offset = EMAOFFSET * gyro_raw + (1 - EMAOFFSET) * self.offset
        speed = (gyro_raw - self.offset) * pi / 180.
        self.angle += speed * interval
        angle_raw = self.get_angle()
        return speed, self.angle # angle_raw * pi / 180. 

class Battery:
    def __init__(self):
        bat_devices = list(pyudev.Context().list_devices(subsystem='power_supply'))
        self.bat_device = bat_devices[0].sys_path
        self.voltage_file = open(os.path.join(self.bat_device, 'voltage_now'), 'r', 0)
        self.current_file = open(os.path.join(self.bat_device, 'current_now'), 'r', 0)
        self.stateValues = [8.,8.]
        self.a = .8
        self.call_ct = 0
        self.voltage_file.seek(0)
        self.voltage = int(self.voltage_file.read())
        self.current = int(self.current_file.read())
        
    def get_voltage(self):
        if self.call_ct < 40:
            self.call_ct +=1
            return self.voltage
        self.call_ct = 0
        self.voltage_file.seek(0)
        self.voltage = int(self.voltage_file.read())
        return self.voltage

    def get_current(self):
        if self.call_ct < 40:
            self.call_ct +=1
            return self.current
        self.call_ct = 0
        self.current_file.seek(0)
        self.current =  int(self.current_file.read())
        return self.current

    def get_batt_denom(self):
        tmp = self.stateValues[0]
        self.stateValues[0] = self.a * self.stateValues[1]\
                               + (1.-self.a) * self.get_voltage()/1000. 
        self.stateValues[1] = tmp
        batt_denom = .001089*self.stateValues[0] - .625
        return batt_denom
    
    def __del__(self):
        self.voltage_file.close()
        self.current_file.close()

def map_range(value, org_range, new_range):
    return (value-org_range[0])/(org_range[1]-org_range[0])*(new_range[1]-new_range[0]) + new_range[0]
        
class IMU:
    def __init__(self):
        IMU_devices = list(pyudev.Context().list_devices(subsystem='lego-sensor').match_attribute('driver_name', 'ms-absolute-imu'))
        self.device_path = IMU_devices[0].sys_path

        self.value1_file = open(os.path.join(self.device_path, 'bin_data'), 'r', 0) # all
        #self.value2_file = open(os.path.join(self.device_path, 'value2'), 'r', 0) # gyro
        #self.value1_file = open(os.path.join(self.device_path, 'value1'), 'r', 0) # tilt
        self.set_poll_ms(30)
        self.set_mode('ALL')
        self.timestamp = 0.0
        self.gyro_ang_old = 0.0

    def __del__(self):
        self.value1_file.close()
        
    def set_mode(self, value):
        with open(os.path.join(self.device_path, 'mode'), 'w') as f:
            f.write(str(value))

    def set_poll_ms(self, value):
        with open(os.path.join(self.device_path, 'poll_ms'), 'w') as f:
            f.write(str(value))

    def calibrate(self):
        with open(os.path.join(self.device_path, 'command'), 'w') as f:
            logging.debug('Calibrating imu...')
            f.write(str('BEGIN-COMP-CAL'))
            f.seek(0)
            time.sleep(0.5)
            f.write(str('END-COMP-CAL'))
            logging.debug("done.")

    def get_ang_and_acc(self):
        self.value1_file.seek(0)
        try:
            bin_data = self.value1_file.read()
        except IOError:
            logging.critical('IMU read error!')
            return 0,0
        
        raw_data = struct.unpack('BBBBBBBBBBBBBBBBBBBBBBbBBBBBBBBB', bin_data)
        gyro_speed = float(raw_data[22])
        gyro_ang = map_range(raw_data[1], [0.0,254.0], [-90.0,90.0])

        if (self.timestamp != 0):
            current_time = time.time()
            gyro_ang_aprox = self.gyro_ang_old + (current_time-self.timestamp) * gyro_speed * 3.5

            #approx_usage_factor = abs(tanh(gyro_speed/40.0))
            approx_usage_factor = (tanh((abs(gyro_speed)-20)/10)+0.9)/1.8
            approx_usage_factor = min(max(approx_usage_factor, 0.0), 1.0)

            gyro_ang_res = (1-approx_usage_factor) * gyro_ang + (approx_usage_factor) * gyro_ang_aprox
            

            #print("%f\t%f\tres=%f\tscaling_factor=%f" % (gyro_ang, gyro_ang_aprox, gyro_ang_res,approx_usage_factor))
            gyro_ang = gyro_ang_res
            self.gyro_ang_old = gyro_ang_res
            self.timestamp = current_time
        else:
            self.gyro_ang_old = gyro_ang
            self.timestamp = time.time()
        
        return [gyro_speed, gyro_ang]


