from acab import fade
from acab import update
import time
t = 5
n = 300
while 1:
    fade(0,255,0,0,t*1000)
    update()
    time.sleep(t)
    print "."
    fade(0,0,255,0,t*1000)
    update()
    time.sleep(t)
    print ","


