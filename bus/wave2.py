import lamplib
import time

def fadeOn(lamp):
    lamplib.fade(lamp,255,255,0,200)

def fadeOff(lamp):
    lamplib.fade(lamp,0,0,255,200)

lamps = ('33','4F','32','31','30','51')
lamp = 0
lamplib.setcolor('FE',0,0,255);
while 1:
    fadeOff(lamps[lamp])
    lamp+=1
    if lamp == len(lamps):
        lamp = 0
    lamplib.fade(lamps[lamp],0,0,127,400)
    time.sleep(0.25)
    lamplib.fade(lamps[lamp],255,0,0,400)
    time.sleep(0.25)

    fadeOn(lamps[lamp])
    time.sleep(5)



