import serial
import time
import getopt
import sys

def wait():
#    time.sleep(1)
    print "wait"
    while(1):
        if ser.read(1) == 'D':
            print "wait done"
            break


lamp = 0
help = False

try:
    opts, args = getopt.getopt(sys.argv[1:], "h:l:", ["help", "lamp="])
except getopt.GetoptError:
    usage()
    sys.exit(2)

for opt, arg in opts:
    if opt in ("-h", "--help"):
        help = True
    elif opt in ("-l", "--lamp"):
        lamp = int(arg)

if help:
    print "Usage: load [OPTION] -l LAMP"
    print "Fade a moodlamp connected to a MLD"
    print "  -l, --lamp=LAMP     Select LAMP instead of broadcast."
    sys.exit(2)

setcolor = "acB%cC\xff\x00\x00ab"%lamp
pause =    "acB%cS\x02ab"%lamp

ser = serial.Serial('/dev/ttyUSB0', 230400)

def write(cmd):
    cmd = "ac"+cmd.replace('a','aa')+"ab"
    print "sending:",list(cmd)
    ser.write(cmd)

#ser.write("acI\x01\x02\x02ab")
#ser.write(pause)
#wait()
r = 255
g = 0
b = 0

while(1):
    if r == 255 and g == 0 and b == 0:
        print "Mark "+str(time.time())
    if r == 0 and not g == 255:
        b -= 1
        g += 1
#        r = 0
    elif g == 0 and not b == 255:
        r -= 1
        b += 1
#        g = 0
    elif b == 0 and not r == 255:
        g -= 1
        r += 1
#        b = 0
#    if lamp == 0:
    setcolor = "B%cC%c%c%c"%(lamp,r,g,b)
#    else:
#        setcolor = "P%cC%c%c%c"%(lamp,r,g,b)

    write(setcolor)
    print "."
    wait()


