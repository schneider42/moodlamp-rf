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
    bus.sendto(cmd,("83.133.178.75",2324))

def fade(lamp, r, g, b, speed):
    h = tohex(int(speed)>>8)
    l = tohex(int(speed)&0xFF)
    cmd = "acF#%s#%s%s%s%s%sab"%(lamp, tohex(r), tohex(g), tohex(b), h, l);
    print cmd
    bus.sendto(cmd,("83.133.178.75",2324))
