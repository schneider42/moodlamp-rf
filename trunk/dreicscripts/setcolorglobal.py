import serial
import time

def wait():
    time.sleep(1)
    while(1):
        if ser.read(1) == 'D':
            break

setcolor = "acB\x00C\xff\x00\x00ab"
pause =    "acB\x00S\x02ab"

ser = serial.Serial('/dev/ttyUSB0', 230400)

ser.write("acI\x01\x02\x02ab")
#ser.write(pause)
#wait()
while(1):
    ser.write(pause)
    wait()
    setcolor = "acB\x00C\x00\xff\x00ab"
    ser.write(setcolor)
    print "."
    wait()
    setcolor = "acB\x00C\xff\x00\x00ab"
    ser.write(pause)
    wait()
    ser.write(setcolor)
    wait()


