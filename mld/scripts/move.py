import sys
import telnetlib
import time
import getopt


def setc(con,lamp,r,b,g):
        s = "003 "+str(lamp)+" "+hex(r)[2:]+" "+hex(g)[2:]+" "+hex(b)[2:]+"\r\n"
        con.write(s)
        s = con.read_until("106", 10)

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
        r1 = 255
        r2 = 0
        b1 = 255
        b2 = 0
        g1 = 255
        g2 = 0
        setc(con,26,r1,b2,g2)
        setc(con,28,r2,b2,g2)
        setc(con,29,r2,b2,g2)
        time.sleep(0.5)
        setc(con,26,r2,b2,g2)
        setc(con,28,r2,b2,g1)
        setc(con,29,r2,b2,g2)
        time.sleep(0.5)
        setc(con,26,r2,b2,g2)
        setc(con,28,r2,b2,g2)
        setc(con,29,r2,b1,g2)
        #s = "003 "+str(lamp)+" "+hex(r)[2:]+" "+hex(g)[2:]+" "+hex(b)[2:]+"\r\n"
        #con.write(s)
        #s = con.read_until("106", 10)
        time.sleep(0.5)


