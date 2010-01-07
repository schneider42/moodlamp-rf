import lamplib
import time
import sys
import random
speed = float(sys.argv[1])
dim = 1
#lamps = ('28','3C','3B','3A','39','38','29','73','33','4F','12','21','1B','2A','32','31','24','23','14','30','10')
lamps = ('28','3C','3B','3A','39','38','29','73','33','4F','32','24','31','30','10')
#lamps = ('33','4F','32','31','30','10')
lamp = 0
l = len(lamps)

lamplib.fade('00',0,0,0,20.*speed)
while 1:
    lamp += 1
    off = lamps[lamp%l]
    on  = lamps[(lamp+5)%l]
    r = int(random.random()*255/dim)
    g = int(random.random()*255/dim)
    b = int(random.random()*255/dim)
    #lamplib.fade(on,0,0,255,30.*speed)
    lamplib.fade(on,r,g,b,10.*speed)
    lamplib.fade(off,0,0,0,10.*speed)
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



