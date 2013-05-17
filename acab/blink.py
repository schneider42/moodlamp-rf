from acab import set
from acab import update
import time
import sys
t = .3
n = 300
lamp = int(sys.argv[1],16)

#for i in range(0,3):
while 1:
    set(lamp,255,0,0)
    update()
    time.sleep(t)
    set(lamp,0,255,0)
    update()
    time.sleep(t)
    set(lamp,0,0,255)
    update()
    time.sleep(t)
    set(lamp,255,255,255)
    update()
    time.sleep(t)

