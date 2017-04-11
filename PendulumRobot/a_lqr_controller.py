#!/usr/bin/env python
import os
import struct
import time
import pyudev
import socket
import sys

from sensors import *
from actors import *
from structs_and_constants import *
from math import *

# enables bluetooth traceing
WITH_BLUETOOTH = False

# These are the main four balance constants, only the gyro constants
# are relative to the wheel size.

# lqr parametrization_2 Q1 new weight
# these are the most stable for now
KPOS=14.1421356236
KSPEED=25.2922525944
KGYROANGLE=33.1338509373
KGYROSPEED=3.34949176826

# If robot power is saturated (over +/- 100) for over this time limit
# then robot must have fallen.  In seconds.
TIME_FALL_LIMIT = .8

# Gyro offset control
# The HiTechnic gyro sensor will drift with time.  This constant is
# used in a simple long term averaging to adjust for this drift.
# Every time through the loop, the current gyro sensor value is
# averaged into the gyro offset weighted according to this constant.
EMAOFFSET = 0.0005

# Friction compensation
# Experimental feature for reducing the oscillatory behaviour
FRICT_COMP = 6.

COLOR_FLIP_INTERVAL = 0.5

class LQRController(object):
    def __init__(self):
        self.gyro_speed = 0.
        self.gyro_angle = 0.
        self.motors_speed = 0.
        self.motors_pos = 0.
        self.bat_volt = 0.
        self.bat_curr = 0.
        self.power = 0.
        self.state = ROBOT_STATES.idle
        self.time_stamp = 0.

    def run(self):



	HOST = ''   # Symbolic name meaning all available interfaces
	PORT = 8888 # Arbitrary non-privileged port
 
	# Datagram (udp) socket
	try :
    	    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            print 'Receiver socket created'
	except socket.error, msg :
            print 'Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
            sys.exit()
 
 
	# Bind socket to local host and port

	try:
    	    s.bind((HOST, PORT))
	except socket.error , msg:
    	    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    	    sys.exit()
        print 'Receiver socket bind complete'
	s.settimeout(0.00001)


	#Sender socket for time sync
	HOST2 = '192.168.1.100'   # Symbolic name meaning all available interfaces
	PORT2 = 8889 # Arbitrary non-privileged port; not the receiver port
 
	# Datagram (udp) socket
	try :
    	    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            print 'Sender socket created'
	except socket.error, msg :
            print 'Failed to sender create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
            sys.exit()
 


        motors = EV3Motors()
        batt = Battery()
        #gyro = Gyro.get_gyro()
        #col  = Color()
        #gyro.calibrate()
        print "balancing in ..."
        for i in range(3, 0, -1):
            print i
            #col.flip_mode()
            time.sleep(0.1)
        print 0
        time.sleep(0.5)

	#Free all packets
	newdata = 0
	while newdata == 0:
	    try:		
	    	d = s.recvfrom(128)
    	    	data = d[0]
    	    	addr = d[1]
    		#data = data.translate(None, '\x00')
	    except socket.timeout:
	    	newdata = 1

        start_time = time.time()
        last_okay_time = start_time
        last_time = start_time
        flip_time_accu = 0.
        avg_interval = 0.010 # 0.0055

	counter = 0
	bat_counter = 10
	cur_interval = 0
	last_time_array = [start_time for i in range(10)]	
	next_time_array = [start_time for i in range(10)]
	passed_time_array = [0. for i in range(10)]
	lastangle = 0.
	angle = 0.
	speed = 0.
	self.power_cam = 0.
	predictedAngle = 0.
	predictedSpeed = 0.
	#positions = [0.0, 0.0]
	#speeds = [0.0, 0.0]
	s2.sendto('1', (HOST2, PORT2))

	while 1:
            try:
		# TIMING STUFF
		last_time_array[0] = time.time()
                self.state = ROBOT_STATES.running
                now_time = time.time()
                self.time_stamp = now_time - start_time
		counter += 1
		if counter == 1000:
		    counter = 0
		    #print (now_time - last_okay_time)
                avg_interval_tmp = avg_interval * 0.99 + 0.01 * (now_time - last_time_array[7])
		#flip_time_accu += (now_time - last_time)
                #avg_interval_tmp = (now_time - last_time)
                #if flip_time_accu >= COLOR_FLIP_INTERVAL:
                #    col.flip_mode()
                #    flip_time_accu = 0.            

                last_time = now_time

		next_time_array[0] = time.time()


		# CHECK GYRO
		last_time_array[1] = time.time()
                #self.gyro_speed, self.gyro_angle = gyro.get_data(avg_interval_tmp)
		next_time_array[1] = time.time()
		
		# CHECK MOTORS
		last_time_array[2] = time.time()
                self.motors_speed, self.motors_pos = motors.get_data(avg_interval_tmp)         
                motors.pos = self.motors_pos		
		next_time_array[2] = time.time()  
		#speeds.append(self.motors_speed)
		#positions.append(self.motors_pos)
		
		# CHECK BATTERY
		#if bat_counter == 1:
		last_time_array[4] = time.time()
                den = batt.get_batt_denom()
                self.voltage = batt.get_voltage()
                self.current = batt.get_current()
		next_time_array[4] = time.time()
		bat_counter = 0
		#bat_counter = bat_counter + 1

		# DATA RECEPTION
		last_time_array[3] = time.time()		
		packetnumber = 0
		data = 0
		newdata = 0
		while newdata == 0:
		    try:		
		    	d = s.recvfrom(128)
    		    	data = d[0]
    		    	addr = d[1]
			packetnumber += 1
    			#data = data.translate(None, '\x00')
		    except socket.timeout:
		    	newdata = 1
		if data:    		
		    try:
        	    	angleSI, speedSI = struct.unpack('>dd',data)
  		        angle = (angleSI + 8.5) * pi / 180
		        speed = (angle-lastangle) / avg_interval_tmp
    		    except struct.error as e:
        	    	print 'ERROR'
        	    	pass
		else:
		    #Predict angle and angle speed based on the mechanical model
		    #First, convert to SI values; speed goes the same way as angle because it results from the angle
		    angleSI = angle * 180 / pi
		    speedSI = speed * 180 / pi
		    #motorspeedSI = 

		    #Next, apply the model
		    angleSI = angleSI + speedSI*avg_interval_tmp;
		    speedSI = speedSI + avg_interval_tmp*(\
			 1250.43 * self.motors_speed \
			+ 242.66 * angleSI \
			- 51.58  * speedSI \
			- 122.35 * self.power)
		    angle = angleSI * pi / 180
		    speed = speedSI * pi / 180
		    predictedAngle = angle
		    predictedSpeed = speed
		    #print "Used prediction"
		#if speed > 100:
		#    speed = 0
		#speed = speed * pi / 180
		lastangle = angle
		next_time_array[3] = time.time()
		
            	
		# CALCULATE POWER
		last_time_array[5] = time.time()               
		self.power_gyro = ( KGYROSPEED * self.gyro_speed
                          + KGYROANGLE * self.gyro_angle) \
                          + KPOS * self.motors_pos \
                          + KSPEED * self.motors_speed
                self.power_cam = ( KGYROSPEED * speed
                          + KGYROANGLE * angle) \
                          + KPOS * self.motors_pos \
                          + KSPEED * self.motors_speed
                
		self.power = self.power_cam * 100./den
                if (abs(self.power) > 100):
                    self.power = self.power/abs(self.power)*100.

                if self.power > 0.:
                    self.power += FRICT_COMP
                elif self.power < 0.:
                    self.power -= FRICT_COMP
		next_time_array[5] = time.time()

            	# CONTROL MOTORS
		last_time_array[6] = time.time()
                left_power, right_power = motors.steer_control(self.power, 0, avg_interval_tmp)
                if(start_time + 1 < now_time):
                    motors.left.set_duty_cycle_sp(int(left_power))
                    motors.right.set_duty_cycle_sp(int(right_power))
		next_time_array[6] = time.time()

		# FALL CHECK
		last_time_array[7] = time.time()           
                if abs(self.power) < 100:
                    last_okay_time = now_time
                elif now_time - last_okay_time > TIME_FALL_LIMIT:
                    #col.set_mode('COL-COLOR')
                    self.state = ROBOT_STATES.fallen
                    break
		next_time_array[7] = time.time()		
		

		#Sending data to the logging PC
		s2.sendto(str(predictedAngle*10000) + ' ' + str(predictedSpeed*10000) + ' ' + str(self.motors_speed*10000) + ' ' + str(self.motors_pos*10000) + ' ' + str(self.power*10000) + ' ' + str(angle*10000) + ' ' + str(speed*10000) + ' ' + str(den*10000), (HOST2, PORT2))
		#print str(predictedAngle) + ' ' + str(predictedSpeed) + ' ' + str(self.motors_speed) + ' ' + str(self.motors_pos) + ' ' + str(self.power) + ' ' + str(angle) + ' ' + str(speed) + ' ' + str(den)

		# PRINT INFO
		if counter == 1000:
		    for i in range(10):
			passed_time_array[i] = next_time_array[i]-last_time_array[i]
		    counter = 0

            except KeyboardInterrupt:
                self.state = ROBOT_STATES.idle
                break
	s2.sendto('0', (HOST2, PORT2))
        motors.left.reset()
        motors.right.reset()
	#print speeds
	#print positions
	print (avg_interval_tmp*1000.0)

    def get_sensor_data(self):
        avg_interval = 0.0055
        data = [0]*len(R2H_PACKET_FORMAT)
        
        data[R2H_PACKET_VARS.STATE] = self.state
        data[R2H_PACKET_VARS.gyro_speed] = self.gyro_speed
        data[R2H_PACKET_VARS.gyro_angle] = self.gyro_angle                                                
        data[R2H_PACKET_VARS.m_speed] = self.motors_speed
        data[R2H_PACKET_VARS.m_position] = self.motors_pos
        data[R2H_PACKET_VARS.voltage]    = self.bat_volt
        data[R2H_PACKET_VARS.current]    = self.bat_curr
        data[R2H_PACKET_VARS.power]      = self.power
        data[R2H_PACKET_VARS.time_stamp] = self.time_stamp
        return data

if __name__ == "__main__":
    

    import time
    
    ctrl = LQRController()


    ctrl.run()
    


