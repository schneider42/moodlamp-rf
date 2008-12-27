import serial
import time
import getopt
import sys


def tohex(val):
    h = hex(val)[2:]
    if val < 0x10 :
        h = '0'+h
    return h.upper()


def wait():
#    time.sleep(0.45)
    while(1):
        if ser.read(1) == 'D':
            break

#r = tohex(int(sys.argv[1]))
#g = tohex(int(sys.argv[2]))
#b = tohex(int(sys.argv[3]))
#h = tohex(int(sys.argv[4])>>8)
#l = tohex(int(sys.argv[4])&0xFF)

setcolor =  "acC#00#000000ab"
#setcolor2 = "acF#4C#%s%s%s%s%sab"%(r,g,b,h,l)

setcolor2 =  "acC#00#0000FFab"
ser = serial.Serial('/dev/ttyUSB0', 115200)
w = float(sys.argv[1])

while(1):
    ser.write(setcolor)
    print setcolor
    wait()
    time.sleep(w)
    ser.write(setcolor2)
    print setcolor2
    wait()
    #time.sleep(float(sys.argv[5]))

    time.sleep(w)
