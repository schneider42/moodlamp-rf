import time
from acab import set
import random

t = 0.1
n = 300
while 1:
    j = 0x27
    for i in range(0x10,0x28):
        set(i,255,0,0)
        set(j,0,255,0)
        time.sleep(t)
        j = i



