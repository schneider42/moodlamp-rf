import lamplib
import time
import sys

speed = float(sys.argv[1])

lamps = ('28','3C','3B','3A','39','38','29')
lamp = 0
l = len(lamps)
while 1:
    lamp += 1
    off = lamps[lamp%l]
    on  = lamps[(lamp+4)%l]
    lamplib.fade(on,255,0,0,20.*speed)
    lamplib.fade(off,0,0,255,30.*speed)
    time.sleep(6./speed)

