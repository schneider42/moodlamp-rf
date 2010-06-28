from acab import fade
import time
t = 1
n = 300
while 1:
    fade(0,255,0,0,t*1000)
    time.sleep(t)
    print "."
    fade(0,0,255,0,t*1000)
    time.sleep(t)
    print ","


