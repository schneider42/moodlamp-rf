import serial
import time
import getopt
import sys

def wait():
    time.sleep(1)
    while(1):
        if ser.read(1) == 'D':
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

setcolor = "acB%cC\x00\xff\x00ab"%lamp
pause =    "acB%cS\x02ab"%lamp

ser = serial.Serial('/dev/ttyUSB0', 230400)

#ser.write("acI\x01\x02\x02ab")
#ser.write(pause)
#wait()
#while(1):
#    ser.write(pause)
#    wait()
#setcolor = "acB%cC\x00\xff\x00ab"%lamp
ser.write(setcolor)
print "."
wait()
#    setcolor = "acB%cC\xff\x00\x00ab"%lamp
#    ser.write(pause)
#    wait()
#    ser.write(setcolor)
#    wait()


