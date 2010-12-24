from acab import set
import time
import sys
t = .3
n = 300
lamp = int(sys.argv[1],16)

while 1:
    set(lamp,255,0,0)
    time.sleep(t)
    set(lamp,0,255,0)
    time.sleep(t)
    set(lamp,0,0,255)
    time.sleep(t)
    set(lamp,255,255,255)
    time.sleep(t)

