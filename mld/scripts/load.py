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
if len(args) > 0:
    filename = args[0]
else:
    print "Usage: load [OPTION] firmware.hex"
    print "Flash the firmware using a MLD to a moodlamp"
    print "  -h, --host=HOST     Connect to HOST. Default", host
    print "  -p, --port=PORT     Use remote port PORT. Default", port
    print "  -l, --lamp=LAMP     Reset LAMP before flashing."
    sys.exit(2)
    
file = open(filename,"r")
con = telnetlib.Telnet(host,port)
con.write("001\r\n")
s = con.read_until(">100", 10)
if s.endswith("100"):
    if lamp != -1:
        print "resetting lamp", lamp
        con.write("013 %d\r\n" % lamp)
        time.sleep(.1)
    print "setting mode to raw"
    con.write("006\r\n")
    s = con.read_until("106", 10)
    if s.endswith("106"):
        print "writing hex file"
        con.write("007\r\n")
        s = con.read_until("106", 10)
        if s.endswith("106"):
            i = 0
            for line in file:
                print "writing line",i
                i+=1
                con.write(line+"\r\n")
            (i, match, text) = con.expect(["105","403"], 10)
            if i == 0:
                print "loaded firmware"
                print "flashing firmware on interface 0"
                
                con.write("009 0\r\n")
                #s = con.read_until("106", 1000)
                (i, match, text) = con.expect(["106","404","405"], 1000)
                #if s.endswith("106"):
                if i == 0:
                    print "flashed ok"
                    print "starting app"
                    con.write("012\r\n")
                elif i == 2:
                    print "error: no such interface"
                else:
                    print "error while flashing:"
                    print text
            else:
                print "error while uploading:", text
print "setting mode to normal"
con.write("011\r\n")    
s = con.read_until("106", 10)
