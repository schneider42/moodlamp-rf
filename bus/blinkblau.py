import time
import sys
import lamplib

if len(sys.argv) > 2:
    lamp = sys.argv[2]
else:
    lamp = "00"

w = float(sys.argv[1])

while(1):
    lamplib.setcolor(lamp, 0, 0, 255)
    time.sleep(w)
    
    lamplib.setcolor(lamp, 0, 0, 0)
    time.sleep(w)
