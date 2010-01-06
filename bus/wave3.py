import lamplib
import time

speed = 4

def fadeOn(lamp):
    lamplib.fade(lamp,255,255,0,300.*speed)

def fadeOff(lamp):
    lamplib.fade(lamp,0,0,127,30.*speed)

lamps = ('28','3C','3B','3A','39','38','29')
#,'12','21','1B','2A','14','1A')
lamp = 0
#lamplib.setcolor('FF',0,0,255);
l = len(lamps)
while 1:
    lamp += 1
    off = lamps[lamp%l]
    on  = lamps[(lamp+4)%l]
    lamplib.fade(on,255,0,0,20.*speed)
    lamplib.fade(off,0,0,255,30.*speed)
    time.sleep(6./speed)

while 1:
    fadeOff(lamps[lamp])
    lamp+=1
    if lamp == len(lamps):
        lamp = 0
    lamplib.fade(lamps[lamp],127,0,127,300.*speed)
    time.sleep(0.25/speed)
    lamplib.fade(lamps[lamp],255,0,0,150.*speed)
    #time.sleep(0.25/speed)

#    fadeOn(lamps[lamp])
    time.sleep(2./speed)



