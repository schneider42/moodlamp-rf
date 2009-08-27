import socket
import time
import getopt
import sys

host = "10.0.0.80"
port = 2324
addr = (host,port)
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

if len(sys.argv) > 1:
    lamp = sys.argv[1]
else:
    lamp = "00"

def tohex(val):
    h = hex(val)[2:]
    if val < 0x10 :
        h = '0'+h
    return h.upper()

def setcolor(lamp, r, g, b):
    cmd = "acC#%s#%s%s%sab"%(lamp,tohex(r),tohex(g),tohex(b))
    print cmd
    s.sendto(cmd,addr)

#r = tohex(int(sys.argv[1]))
#g = tohex(int(sys.argv[2]))
#b = tohex(int(sys.argv[3]))
#h = tohex(int(sys.argv[4])>>8)
#l = tohex(int(sys.argv[4])&0xFF)

w = float(sys.argv[2])

while(1):
    setcolor(lamp, 0, 0, 255)
    time.sleep(w)
    
    setcolor(lamp, 0, 0, 0)
    time.sleep(w)
