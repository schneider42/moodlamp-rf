import time
from acab import set
import random
r = g = b = cr = cg = cb = 0

def recolor():
    r = int(random.random()*255)
    g = int(random.random()*255)
    b = int(random.random()*255)
    cr = 255-r
    cg = 255-g
    cb = 255-b

t = 0.05
n = 300
while 1:
    recolor();
    for i in range(0x10,0x28):
        set(i,r,g,b)
        time.sleep(t)
    recolor();
    for i in range(0x28,0x10,-1):
        set(i,r,g,b)
        time.sleep(t)



