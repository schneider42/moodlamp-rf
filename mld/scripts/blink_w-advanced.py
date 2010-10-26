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


import getopt
import sys
import telnetlib
import time

def setc(con,lamp,r,g,b):
    r = int(r)
    g = int(g)
    b = int(b)
    s = "003 "+str(lamp)+" "+hex(r)[2:]+" "+hex(g)[2:]+" "+hex(b)[2:]+"\r\n"
    con.write(s)
    s = con.read_until("106", 10) #self.telnet.read_until(expected,timeout)


port = 2324
host = "localhost"
lamp = -1
iteration = 100
red = 255
green = 255
blue = 255
mycounter = 500 #5000 #8.13min

try:
    opts, args = getopt.getopt(sys.argv[1:], "h:p:l:i:r:g:b:", ["host=", "port=", "lamp=", "iteration=", "red=", "green=", "blue="])
except getopt.GetoptError:          
    usage()                         
    sys.exit(2)

for opt, arg in opts:
    if opt in ("-h", "--host"):
        host = arg
    elif opt == ("-p", "--port"):
        port = int(arg)
    elif opt in ("-l", "--lamp"):
        lamp = arg
    elif opt in ("-i", "--iteration"):
        iteration = int(arg)
    elif opt in ("-r", "--red"):
        red = int(arg)
    elif opt in ("-g", "--green"):
        green = int(arg)
    elif opt in ("-b", "--blue"):
        blue = int(arg)


if lamp == -1:
    print "Usage: load [OPTION] -l LAMP"
    print " from 0 to COLOR VALUE blinking moodlamps connected to a MLD"
    print "  -h, --host=HOST     Connect to HOST. Default", host
    print "  -p, --port=PORT     Use remote port PORT. Default", port
    print "  -l, --lamp=LAMP     Reset LAMP before flashing."
    print "  -i, --iteration=TIME     set time/counter"
    print "  -r, --red=VALUE     set red value"
    print "  -g, --green=VALUE     set red value"
    print "  -b, --blue=VALUE     set red value"
    print "examples:"
    print "python blink_w-advanced.py -r 0 -g 255 -b 0 -l 0 -i 42 &"
    print "python /root/mld/scripts/green_w.py -l NAME"
    sys.exit(2)
   

con = telnetlib.Telnet(host, port)
con.write("001\r\n")
s = con.read_until(">100", 10)
if s.endswith("100"):
    while iteration > 0:
        iteration -= 1

        r = 0
        g = 0
        b = 0
        setc(con,lamp,r,g,b)

#	time.sleep(1)
        
	r = red
        g = green 
        b = blue 
        setc(con,lamp,r,g,b)

