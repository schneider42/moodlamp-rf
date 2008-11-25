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

while True:
    c = ser.read(1)
    if escaped:
        if c == '0':
            start = True
        elif c == '1':
            stop = True
        elif c == '\\':
            d = '\\'
        escaped = False
    elif c == '\\':
        escaped = 1
    else:
        d = c
        
    if start:
        start = False
    elif stop:
        print time.time(), data
        data = []
        stop = False
    elif escaped == False:
        data.append(d)
        
    

