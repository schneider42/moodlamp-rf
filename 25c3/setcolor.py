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

val = int(sys.argv[1])

#r = g = b = tohex(val)
r = tohex(int(sys.argv[1]))
g = tohex(int(sys.argv[2]))
b = tohex(int(sys.argv[3]))
setcolor = "acC#00#%s%s%sab"%(r,g,b)

ser = serial.Serial('/dev/ttyUSB0', 115200)

#while(1):
ser.write(setcolor)
#print setcolor
wait()
#ser.write(setcolor)
#wait()
#    time.sleep(0.2)
    #val+=1
#    r = g = b =  tohex(val)
#    g = tohex(val+1)
#    b = tohex(val+2)
#    setcolor = "acC#4C#%s%s%sab"%(r,g,b)
#    print val

