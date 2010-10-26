#import serial
import socket
import sys

out = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
out.connect(('83.133.178.74',2000))

def sendline(out, line):
    print "sending:",line
    while True:
        out.send(line)
        c = ' '
        while c != 'D' and c != '':
            c = out.recv(1)
            print list(c)
        if c == 'D':
            return
        elif c == '':
            out = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            out.connect(('83.133.178.74',2000))

def setadr(line,adr):
    return line[:4]+adr+line[6:]

input = ''
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(('', 2324))

central = ('32','31','30','41','24','33','4f','51')
wipp = ('21','16','2A','12')
media = ('14','1A')
while True:
    try:
        while True:
            (input, src) = s.recvfrom(255)
            print input, src
            id =  input[4:6]
            lamps = []
            if id == 'FD':
                lamps += wipp
            elif id == 'FE':
                lamps += central
            elif id == 'FC':
                lamps += media
            elif id == 'EF':
                lamps += wipp + central
            elif id == 'EE':
                lamps += central + media
            elif id == 'ED':
                lamps += wipp + media
            else:
                sendline(out, input)

            print (input,)*3
            lamps = map(setadr,(input,)*len(lamps),lamps)

            map(sendline,(out,)*len(lamps),lamps)
    except Exception:
        print "Something went wrong..."
        print sys.exc_info()
        out = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        out.connect(('83.133.178.74',2000))



