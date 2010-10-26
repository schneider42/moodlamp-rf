import serialinterface
import time

serial = serialinterface.SerialInterface("/dev/ttyUSB0", 115200, 1);

def high(x):
    return (x>>8)&0xff;

def low(x):
    return x&0xff;

def set(lamp,r,g,b):
    serial.write("\x5c\x30%cC%c%c%c\x5c\x31"%
        (chr(lamp),chr(r),chr(g),chr(b)))

def fade(lamp,r,g,b,ms):
    serial.write("\x5c\x30%cM%c%c%c%c%c\x5c\x31"%
        (chr(lamp),chr(r),chr(g),chr(b),chr(high(ms)),chr(low(ms))))

