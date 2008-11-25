import sys
import telnetlib
import time
import getopt
import random
import math

port = 2324
host = "localhost"
lamp = -1

def setc(con,lamp,r,b,g):
        s = "003 "+str(lamp)+" "+hex(r)[2:]+" "+hex(g)[2:]+" "+hex(b)[2:]+"\r\n"
        con.write(s)
        s = con.read_until("106", 10)

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
    a = a1 = a2 = 0.0
    while True:
        r = int( math.sin(a%6.28)*127+127)
        g = int( math.sin(a1%6.28)*127+127)
        b = int( math.sin(a2%6.28)*127+127)
        
        a+=random.random() *0.01
        a1+=random.random() *0.015 #0.02
        a2+=random.random() *0.02 #0.03

        setc(con,lamp,r,b,g)
        time.sleep(0.1)
        
