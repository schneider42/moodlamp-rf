import threading
import serial
import string
import curses
import sys
import time
import math
import random

ser =  serial.Serial('/dev/ttyUSB1', 115200)

data = []
#ser.write(aus)
escaped = False
stop = False
start = False
inframe = False
while True:
    c = ser.read(1)
#    print "c=",ord( c)
#    continue
    if escaped:
        if c == '0':
            start = True
            inframe = True
            print "start"
        elif c == '1':
            stop = True
            inframe = False
        elif c == '\\':
            d = '\\'
        else:
            print "out of order escape"
        escaped = False
    elif c == '\\':
        escaped = 1
    else:
        d = c
        
    if start:
        start = False
    elif stop:
        print time.time(), len(data) ,data
        data = []
        stop = False
    elif escaped == False and inframe:
        data.append(d)
    elif escaped == False:
        print "c=",ord( c)

