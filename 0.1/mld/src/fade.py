import sys
import telnetlib
import time
import getopt

port = 2324
host = "localhost"
lamp = -1

try:
    opts, args = getopt.getopt(sys.argv[1:], "h:p:l:", ["host=", "port=", "lamp="])
except getopt.GetoptError:          
    usage()                         
    sys.exit(2)                     
for opt, arg in opts:
    if opt in ("-h", "--host"):
        host = arg
    elif opt == ("-p", "--port"):
        port = int(arg)
    elif opt in ("-l", "--lamp"):
        lamp = int(arg)
if lamp == -1:
    print "Usage: load [OPTION] -l LAMP"
    print "Fade a moodlamp connected to a MLD"
    print "  -h, --host=HOST     Connect to HOST. Default", host
    print "  -p, --port=PORT     Use remote port PORT. Default", port
    print "  -l, --lamp=LAMP     Reset LAMP before flashing."
    sys.exit(2)
    

con = telnetlib.Telnet(host,port)
con.write("001\r\n")
s = con.read_until(">100", 10)
if s.endswith("100"):
#if True:
    r = 255
    g = 0
    b = 0
    while True:
        if r == 255 and g == 0 and b == 0:
            print "Mark "+str(time.time())

        if r == 0 and not g == 255:
            b -= 1
            g += 1
        elif g == 0 and not b == 255:
            r -= 1
            b += 1
        elif b == 0 and not r == 255:
            g -= 1
            r += 1
        s = "003 "+str(lamp)+" "+hex(r)[2:]+" "+hex(g)[2:]+" "+hex(b)[2:]+"\r\n"
        #print s
        con.write(s)
        s = con.read_until("106", 10)
        #con.write("003 "+str(lamp)+" 00 FF 00\r\n")
        #s = con.read_until("106", 10)
