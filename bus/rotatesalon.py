import socket
import time
import getopt
import sys

host = "10.0.0.80"
port = 2324
addr = (host,port)
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
lamps = ["2B", "1C", "11", "2C"]
w = float(sys.argv[1])
i = int(sys.argv[2])

def tohex(val):
    h = hex(val)[2:]
    if val < 0x10 :
        h = '0'+h
    return h.upper()

def setcolor(lamp, r, g, b):
    cmd = "acC#%s#%s%s%sab"%(lamp,tohex(r),tohex(b),tohex(g))
    print cmd
    s.sendto(cmd,addr)

def nextlamp(l):
    return (l+1)%len(lamps)

def getoposite(l):
    return (l+2)%len(lamps)

v = 0
l = 0

while(1):
    setcolor(lamps[l], v, 0, 255-v)
    time.sleep(w)
    setcolor(lamps[nextlamp(l)], 255-v, 0, v)
    time.sleep(w)
    setcolor(lamps[getoposite(l)], v, 0, 255-v)
    time.sleep(w)
    setcolor(lamps[getoposite(nextlamp(l))], 255-v, 0, v)

    v = (v+i)%256
    if( v < i ):
        #setcolor(lamps[l],0,0,0)
        l = nextlamp(l)
        #setcolor(lamps[l],0,0,0)
    time.sleep(w)
