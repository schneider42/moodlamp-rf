 # Copyright (c) 2008 by Tobias Schneider <schneider@blinkenlichts.net>
 # Copyright (c) <year> by your name <your@email>
 #
 # This program is free software: you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program.  If not, see <http://www.gnu.org/licenses/>.


import sys
import telnetlib
import time
import getopt

def setc(con,lamp,r,g,b):
    r = int(r)
    g = int(g)
    b = int(b)
    s = "003 "+str(lamp)+" "+hex(r)[2:]+" "+hex(g)[2:]+" "+hex(b)[2:]+"\r\n"
    con.write(s)
    s = con.read_until("106", 10)

def usage():
    print ""
    print "Usage:", sys.argv[0], "[OPTION] -l LAMP"
    print "sends continously: set color white "
    print "  -h, --host=HOST     Connect to HOST. Default", host
    print "  -p, --port=PORT     Use remote port PORT. Default", port
    print "  -l, --lamp=LAMP     address or name, -l 0 for all"
    print "  -d, --debug         Enable debugging"
    print "  -u, --usage         print help"

port = 2324
host = "localhost"
lamp = -1
debug = "false"

try:
    opts, args = getopt.getopt(sys.argv[1:], "h:p:l:du", ["host=", "port=", "lamp=", "debug", "usage"])
    #print "debug args:", args
    #print "debug opts:", opts
except getopt.GetoptError:          
    print "getopt Error, cant define opts,arg"
    usage()                         
    sys.exit(2)

for opt, arg in opts:
    if opt in ("-h", "--host"):
        host = arg
    elif opt in ("-p", "--port"):
        port = int(arg)
    elif opt in ("-l", "--lamp"):
        lamp = arg
    elif opt in ("-d", "--debug"):
        debug = "true"  
    elif opt in ("-u", "--usage"):
        usage()
        sys.exit(2)
    else:
        print "unknown option"
        usage()
        sys.exit(2)

if lamp == -1:
    usage()
    sys.exit(2)

if debug == "true":
    logging.basicConfig(level=logging.DEBUG,format="%(levelname)s:  %(message)s")
else:
    logging.basicConfig(level=logging.INFO,format="%(levelname)s:  %(message)s")


con = telnetlib.Telnet(host, port)
con.write("001\r\n")
s = con.read_until(">100", 10)
if s.endswith("100"):
    while True:
        r = 255#220
        g = 255#255
        b = 255#220
        setc(con,lamp,r,g,b)
        time.sleep(0.5)
        
        r = 255
        g = 255#255
        b = 255
        setc(con,lamp,r,g,b)
        time.sleep(0.5)
        
        """r = 0
        g = 0
        b = 255
        setc(con,lamp,r,g,b)
        time.sleep(0.05)"""
else:
    logging.error("initial mld test command: 001 failed")



