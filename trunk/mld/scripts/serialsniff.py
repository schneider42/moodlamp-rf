import threading
import serial
import string
import curses
import sys
import time
import math
import random

ser =  serial.Serial('/dev/ttyUSB1', 230400)

data = []
#ser.write(aus)
escaped = False
stop = False
start = False

while True:
    c = ser.read(1)
#    print "c=", c
#    continue
    if escaped:
        if c == 'c':
            start = True
        elif c == 'b':
            stop = True
        elif c == 'a':
            d = 'a'
        escaped = False
    elif c == 'a':
        escaped = 1
    else:
        d = c
        
    if start:
        start = False
    elif stop:
        print time.time(), len(data) ,data
        data = []
        stop = False
    elif escaped == False:
        data.append(d)
        
    

