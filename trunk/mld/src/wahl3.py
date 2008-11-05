import urllib2
import re
import getopt
import sys
import telnetlib
import time
import math
import threading

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

rep = None
dem = None
won = 0
class Timer ( threading.Thread ):
    def __init__ ( self):
        threading.Thread.__init__ ( self )
        self.cnn = CNN()
    def run ( self ):
        while 1:
            global rep
            global dem
            global won
            try:
                d,r = self.cnn.get()
                print d,r
                if d != None and r != None:
                    dem = d
                    rep = r
                else:
                    print "none"
                time.sleep(10)
            except ElectionWon, elewon:
                if(elewon.message == 'D'):
                    won = 'd'
                elif(elewon.message == 'R'):
                    won = 'r'
                return
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
    

con = telnetlib.Telnet(host, port)
con.write("001\r\n")
s = con.read_until(">100", 10)

con.write("004 %d\r\n" % lamp)
con.write("004 %d\r\n" % 10)
Timer().start()

period = 5
  
if s.endswith("100"):
    r = 10
    g = 10
    b = 10
    while True:
    #    try:
            #dem,rep = cnn.get()
            #print dem,rep
            #print normalize(dem,rep)
            if rep != None and dem != None :
                done = float(rep[1] + dem[1])
                if rep[1] > dem[1]:
                    max = float(rep[1])
                else:
                    max = float(dem[1])
                rp = rep[1]/max
                dp = dem[1]/max
                print normalize(dem[1],rep[1])
                #rp /= 10.
                #dp /= 10.

                setc(con,lamp,255,0,255)
                setc(con,10,255,0,255)
                if rp > dp:
                    setc(con,lamp,255,0,0)
                    setc(con,10,255,0,0)
                    time.sleep(dp * period);

                    setc(con,lamp,255,0,0)
                    setc(con,10,255,0,0)
                    time.sleep(period - dp*period)
                else:

                    time.sleep(rp * period)
                    setc(con,lamp,0,0,255)
                    setc(con,10,0,0,255)
                    time.sleep(period - rp*period)

            else:
                time.sleep(1)

            if won:
                if won == 'd':
                    while 1:
                        setc(con,lamp,0,0,255)
                        setc(con,10,0,0,255)
                        time.sleep(1)
                        setc(con,lamp,0,0,0)
                        setc(con,10,0,0,0)
                        time.sleep(1)

                else:
                    while 1:
                        setc(con,10,255,0,0)
                        setc(con,lamp,255,0,0)
                        time.sleep(1)
                        setc(con,10,0,0,0)
                        setc(con,lamp,0,0,0)
                        time.sleep(1)
#                setc(con,lamp,0,0,0)
#                setc(con,10,0,0,0)
#        con.write("004 %d\r\n" % lamp)
#        except NoneType:
#            print "none"
#            pass
#            time.sleep(5)

  
