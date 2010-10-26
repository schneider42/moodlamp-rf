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


import urllib2
import re
import getopt
import sys
import telnetlib
import time
import math

port = 2324
host = "localhost"
lamp = -1

class ElectionWon(Exception):
  pass

class CNN(object):
  def __init__(self):
    self.url = "http://www.cnn.com/ELECTION/2008/results/president/"

  def get(self):
    """return the electoral balance
        (dpopular, delectoral), (rpopular, relectoral)
    """
    d,r = None, None
    u = urllib2.urlopen(self.url)
    for line in u.readlines():
      res = re.search(r'var CNN_NData=(.*?);', line)
      if res is not None:
        data = res.group(1)
        data = data.replace("true", "True")
        data = data.replace("false", "False")
        data = eval(data)

        d,r = None, None
        for candidate in data['P']['candidates']:
          if candidate['party'] == 'D':
            d = candidate['votes'], candidate['evotes']
            if candidate['winner']:
              raise ElectionWon("D")
          elif candidate['party'] == 'R':
            r = candidate['votes'], candidate['evotes']
            if candidate['winner']:
              raise ElectionWon("R")

    return d,r

def normalize(d,r, interval=0.5):
  """normalize to
    -1 => democrat winning
    +1 => republican winning
    abs(1.0) is having "2*interval" higher percent than the other guy
  """
  if d+r == 0: return 0.0
  rnorm = 2.0*(1.0*r/(d+r) - 0.5) / (2*interval)
  if rnorm > 1.0: rnorm = 1.0
  if rnorm < -1.0: rnorm = -1.0
  return rnorm
  
def setc(con,lamp,r,g,b):
    r = int(r)
    g = int(g)
    b = int(b)
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
    elif opt in ("-p", "--port"):
        port = int(arg)
    elif opt in ("-l", "--lamp"):
        lamp = arg

if lamp == -1:
    print "Usage: load [OPTION] -l LAMP"
    print "Fade a moodlamp connected to a MLD"
    print "  -h, --host=HOST     Connect to HOST. Default", host
    print "  -p, --port=PORT     Use remote port PORT. Default", port
    print "  -l, --lamp=LAMP     Reset LAMP before flashing."
    sys.exit(2)
    

con = telnetlib.Telnet(host, port)
con.write("001\r\n")
s = con.read_until(">100", 10)

con.write("004 %d\r\n" % lamp)

cnn = CNN()
print cnn.get()
  
if s.endswith("100"):
    r = 10
    g = 10
    b = 10
    while True:
    #    try:
            dem,rep = cnn.get()
            print dem,rep
            #print normalize(dem,rep)
            if rep != None and dem != None :
                done = float(rep[1] + dem[1])
                rp = rep[1]/done * 100
                dp = dem[1]/done * 100
#            v = float(rep[1]) / float(dem[1])
                rdiff = rp - 40;
                ddiff = dp - 40;

                if(rdiff > 20):
                    rdiff = 20
                if(ddiff > 20):
                    rdiff = 20

                if(rdiff < 0):
                    rdiff = 0;
                if(ddiff < 0):
                    ddiff = 0

                rdiff *= 255./20
                ddiff *= 255./20
                print rdiff, ddiff

                rp /= 10.
                dp /= 10.


                setc(con,lamp,255,0,0)
                setc(con,10,0,0,255)
                if rp > dp:
                    time.sleep(dp);
                    setc(con,10,0,0,0)
                    time.sleep(10. - dp)
                else:
                    time.sleep(rp)
                    setc(con,lamp,0,0,0)
                    time.sleep(10.- rp)

                
                setc(con,lamp,0,0,0)
                setc(con,10,0,0,0)
#        con.write("004 %d\r\n" % lamp)
#        except NoneType:
            print "none"
            pass
            time.sleep(5)

  
