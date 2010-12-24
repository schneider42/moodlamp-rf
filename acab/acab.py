import serialinterface
import time

serial = serialinterface.SerialInterface("/dev/ttyUSB0", 115200, 1);

def high(x):
    return (x>>8)&0xff;

def low(x):
    return x&0xff;

def set(lamp,r,g,b):
    cmd = "%cC%c%c%c"%(chr(lamp),chr(r),chr(g),chr(b))
    cmd = cmd.replace("\\","\\\\")
    serial.write("\x5c\x30%s\x5c\x31"%cmd);

def fade(lamp,r,g,b,ms):
    cmd = "\x5c\x30%cM%c%c%c%c%c\x5c\x31"%(chr(lamp),chr(r),chr(g),chr(b),chr(high(ms)),chr(low(ms)))
    cmd = cmd.replace("\\","\\\\")
    serial.write("\x5c\x30%s\x5c\x31"%cmd);

