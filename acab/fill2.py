import time
import acab
import random
r = g = b = cr = cg = cb = 0
lamps = [0x2A ,0x30 ,0x10]

def recolor():
    global r,g,b
    r = int(random.random()*255)
    g = int(random.random()*255)
    b = int(random.random()*255)
    cr = 255-r
    cg = 255-g
    cb = 255-b

t = 700
n = 300
while 1:
    recolor();
    #for i in range(0x10,0x28):
    for i in range(0,3):
        acab.fade(lamps[i],r,g,b,t)
        time.sleep(t/1000./2.)
    recolor();
    for i in range(2,-1,-1):
    #for i in range(0x28,0x10,-1):
        acab.fade(lamps[i],r,g,b,t)
        time.sleep(t/1000./2.)

