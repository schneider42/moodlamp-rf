import serial
import time

def wait():
    time.sleep(1)
    while(1):
        if ser.read(1) == 'D':
            break

ser = serial.Serial('/dev/ttyUSB0', 230400)
ser.write("acI\x01\x02\x02ab")

