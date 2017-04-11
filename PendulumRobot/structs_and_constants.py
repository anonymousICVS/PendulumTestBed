
import struct

class H2R_PACKET_VARS:
    command             = 0
    M1_duty_cycle_sp    = 1
    M2_duty_cycle_sp    = 2
    M1_command          = 3
    M2_command          = 4
#
# Variable data types
H2R_PACKET_FORMAT   = 'BbbBB'
# Packet size
H2R_PACKET_SIZE   = struct.calcsize(H2R_PACKET_FORMAT)

# Variable names for packet cells
class R2H_PACKET_VARS:
    STATE           = 0
    gyro_speed      = 1
    gyro_angle      = 2
    imu_speed       = 3
    imu_angle       = 4
    m_position      = 5
    m_speed         = 6
    voltage         = 7
    current         = 8
    power           = 9
    time_stamp      = 10
    names           = ['STATE',  'gyro_speed', 'gyro_angle', 'imu_speed' ,\
                       'imu_angle', 'm_position', 'm_speed', 'voltage', 'current',\
                       'power', 'time_stamp']

# Variable data types
R2H_PACKET_FORMAT   = 'Bffffffffff'
# Packet size
R2H_PACKET_SIZE   = struct.calcsize(R2H_PACKET_FORMAT)

# Robot states
class ROBOT_STATES:
    idle    = 0
    running = 1
    fallen  = 2

# Robot commands
class ROBOT_COMMANDS:
    none   = 0
    stop   = 1
    start  = 2
    march  = 3
    turnl  = 4
    turnr  = 5

