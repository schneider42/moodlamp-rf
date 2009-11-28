import lamplib
import time

speed = 50.

def fadeOn(lamp):
    lamplib.fade(lamp,255,255,0,200*speed)

def fadeOff(lamp):
    lamplib.fade(lamp,0,0,255,200*speed)

lamps = ('31','30','51','33','4F','32')
#,'12','21','1B','2A','14','1A')
lamp = 0
lamplib.setcolor('FE',0,0,255);
while 1:
    fadeOff(lamps[lamp])
    lamp+=1
    if lamp == len(lamps):
        lamp = 0
    lamplib.fade(lamps[lamp],0,0,127,400*speed)
    time.sleep(0.25/speed)
    lamplib.fade(lamps[lamp],255,0,0,400*speed)
    time.sleep(0.25/speed)

#    fadeOn(lamps[lamp])
    time.sleep(2/speed)



