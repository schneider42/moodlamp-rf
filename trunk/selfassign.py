import serial
import time

def wait():
    time.sleep(1)
    while(1):
        if ser.read(1) == 'D':
            break

autoassign = "acB\x00Xab"
pause = "acB\x00S\x07ab"
ser = serial.Serial('/dev/ttyUSB0', 230400)

#ser.write("acI\x01\x02\x02ab")
#ser.write(pause)
#wait()
while(1):
    ser.write(autoassign)
    wait()
    ser.write(pause)
    wait()


