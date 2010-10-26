from acab import set
import time
t = 1
n = 300
while 1:
    set(0,255,0,0)
    time.sleep(t)
    print "."
    set(0,0,255,0)
    time.sleep(t)
    print ","


