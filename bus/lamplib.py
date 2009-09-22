import socket
bus = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

def tohex(val):
    h = hex(val)[2:]
    if val < 0x10 :
        h = '0'+h
    return h.upper()

def setcolor(lamp, r, g, b):
    cmd = "acC#%s#%s%s%sab"%(lamp,tohex(r),tohex(g),tohex(b))
    print cmd
    bus.sendto(cmd,("10.0.0.80",2324))
