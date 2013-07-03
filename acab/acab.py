import serialinterface
import time

serials = [
    "/dev/ttyUSB0",
#    "/dev/ttyUSB1",
#    "/dev/ttyUSB2",
#    "/dev/ttyUSB3",
#    "/dev/ttyUSB4",
#    "/dev/ttyUSB5",
#    "/dev/ttyUSB6",
#    "/dev/ttyUSB7",
    ]

def createSerial(dev):
    ser = serialinterface.SerialInterface(dev,115200,1)
    # put the bridge into full drive mode
    # ser.write('\\F')
    return ser

serials = map(createSerial,serials)

def high(x):
    return (x>>8)&0xff;

def low(x):
    return x&0xff;

def set(lamp,r,g,b):
    cmd = "%cP%c%c%c"%(chr(lamp),chr(r),chr(g),chr(b))
    cmd = "\x5c\x30%s\x5c\x31"%cmd
    cmd = cmd.replace("\\","\\\\")
    for serial in serials:
        serial.write(cmd);

def fade(lamp,r,g,b,ms):
    cmd = "%cc%c%c%c%c%c"%(chr(lamp),chr(r),chr(g),chr(b),chr(high(ms)),chr(low(ms)))
    cmd = cmd.replace("\\","\\\\")
    cmd = "\x5c\x30%s\x5c\x31"%cmd
    cmd = cmd.replace("\\","\\\\")
    for serial in serials:
        serial.write(cmd);

def update():
    cmd = '%cU'%chr(0)
    cmd = "\x5c\x30%s\x5c\x31"%cmd
    cmd = cmd.replace("\\","\\\\")
    for serial in serials:
        serial.write(cmd);

def reset(lamp):
    cmd = '%cB'%chr(lamp)
    cmd = "\x5c\x30%s\x5c\x31"%cmd
    cmd = cmd.replace("\\","\\\\")
    for serial in serials:
        serial.write(cmd);


