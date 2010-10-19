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


#from __future__ import with_statement

daemonize = 'false' #true/false
serial = "/dev/ttyUSB0"
force = "true"
pidfile = "/tmp/mld.pid"
log = "false"
port = int(2324)

import sys, os
import asyncore
import rf12interface
import Timer
import moodlamp
import mldclient
import getopt
import atexit
import logging
from signal import SIGTERM 
import time

class MLD:
    def serve(self, mport):
        self.ml = moodlamp.MoodlampList()
        self.interfaces = []
        try:
            self.interfaces.append(rf12interface.RF12Interface(str(serial), 230400, 1, 2, self))
        except: #TODO error type
            logging.error("can't connect to serial device: %s", serial)
            if force == "true":
                logging.info("force = true, starting without serial device")
            else:
                logging.info("force = false, mld refuses to start without serial device")
                sys.exit(2)
            #self.interfaces.append(rf12interface.RF12Interface("/dev/ttyS0",115200,2,self))
        self.server = mldclient.ClientServer(mport, self.ml, self.interfaces, self)
        #self.interfaces[0].start()
        Timer.Timer(1, self).start()
        
        asyncore.loop()

    def new_lamp(self, lamp):
        self.server.new_lamp_detected(lamp)
        
    def remove_lamp(self, lamp):
        logging.info("deleting lamp %s ", lamp)
        self.ml.lock.acquire()
        for m in self.ml:
            if m.address == lamp:
                """if m is Moodlamp:
                    print "remove lamp", m.address, " normal"
                    while(1):
                        pass
                else:
                    print "remove dummy", m.address, " normal"
                    while(1):
                        pass"""
                self.ml.remove(m)
        self.ml.lock.release() 
    
    def isinlist(self, adr):
        for m in self.ml:
            if m.address == adr:
                return True
        return False
    
    def getNewAddress(self):
        adr = 10
        while self.isinlist(adr):
            adr += 1
        if adr > 255:
            adr = 255    
        return adr
    
    def new_packet(self, adr, data, broadcast):
        self.ml.lock.acquire()
        if data[0] == 'R':
            if adr == 0:
                adr = self.getNewAddress()
            dummy = moodlamp.DummyLamp()
            dummy.address = adr
            self.ml.append(dummy)
            reply = "x"+"".join(data[1:])+("\x00%c%c\x01" %(adr, 0))

            if broadcast == True:
                adr = 0
            self.interfaces[0].packet( adr, reply, broadcast, True)
            self.ml.lock.release()
            return
        
        if data[0] == 'I':
            self.remove_lamp(adr)
            name = "".join(data[1:])
            lamp = moodlamp.Moodlamp(self.interfaces[0], self, adr, name)
            self.ml.append(lamp)
            logging.info("len ml: %s", len(self.ml))
            self.ml.lock.release()
            return
        
        """if adr == 0:# and broadcast == True:
            lamp = Moodlamp(self.interfaces[0], self)
            if lamp.setup(data):
                print "setup ok"
                self.ml.append(lamp)
                print "len ml:"+str(len(self.ml))
            self.ml.lock.release()
            return
        """
        unknown = 1
        for m in self.ml:
            if m.address == adr:
                m.data(data, broadcast)
                unknown = 0
                break
        if unknown == 1:
            logging.info("resetting lamp %s", adr)
            self.interfaces[0].packet( adr, "r", 0, True)        #force reset for unknown lamp
                                                                #todo start rebinding
        self.ml.lock.release()
        
    def packet_done(self, adr, broadcast):
        #self.ml.lock.acquire()                            #Todo use a queue or something to prevent blocking
        logging.info("packet done adr= %s" , adr)
        logging.info("len ml: %s", len(self.ml))
        for m in self.ml:
            #print "iter ml adr="+str(m.address)
            if m.address == adr:
                #print "adr ok"
                m.packet_done(broadcast)
                break
        #self.ml.lock.release()

    def timer(self):
        #with self.lock:
        self.ml.lock.acquire()
        for m in self.ml:
            m.timer -= 1
            if m.timer <= 0:
                if m is not moodlamp.DummyLamp:
                    self.server.lamp_removed(m)
                self.ml.remove(m)
                
        self.ml.lock.release()   

    def flash(self, interface, firmware):
        self.ml.lock.acquire()
        self.ml.lock.release()
        #unused interface and firmware

def daemon(stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
    try:
        if os.fork() > 0:
            print 'first fork, ok'
            sys.exit(0)
    except OSError, error:
        print 'fork #1 failed: %d (%s)' % (error.errno, error.strerror)
        sys.exit(1)
    os.chdir('.')
    os.setsid()
    os.umask(0)
    try:
        pid = os.fork()
        if pid > 0:
            print 'mld daemon pid %d' % pid
            sys.exit(0)
    except OSError, error:
        print 'fork #2 failed: %d (%s)' % (error.errno, error.strerror)
        sys.exit(1)

    if log !=  "true":  
        for f in sys.stdout, sys.stderr:
            f.flush( )
        si = file(stdin, 'r')
        so = file(stdout, 'a+')
        se = file(stderr, 'a+', 0)
        os.dup2(si.fileno( ), sys.stdin.fileno( ))
        os.dup2(so.fileno( ), sys.stdout.fileno( ))
        os.dup2(se.fileno( ), sys.stderr.fileno( ))
                

        # Check for a pidfile to see if the daemon already runs
#
    try:
        pf = file(pidfile,'r')
        pid = int(pf.read().strip())
        pf.close()
    except IOError:
        pid = None
#
    if pid:
        message = "pidfile %s already exist. Daemon already running?\n"
        sys.stderr.write(message % pidfile)
        try:
           while 1:
               os.kill(pid,SIGTERM)
               time.sleep(1)
        except OSError, err:
           err = str(err)
           if err.find("No such process") > 0:
               os.remove(pidfile)

    atexit.register(delpid)
    pid = str(os.getpid())
    file(pidfile,'w+').write("%s\n" % pid)
    file(pidfile).close()
    #pfile = open(pidfile,'w')
    #pfile.write("%s\n" % pid)
    #pfile.close()
    print "register pidfile: %s", pidfile
    #atexit.register(os.remove(pidfile))

    MLD().serve(port)

def delpid():
    logging.info("delete pidfile")
    os.remove(pidfile)

def usage():
    print ""
    print "Usage:", sys.argv[0], "[OPTION]"
    print "fireup mld daemon"
    print "  -i, --serial=DEVICE     Connect to Device. Default:", serial
    print "  -p, --port=PORT         set port. Default:", port
    print "  -e, --pidfile=PIDFILE   set pidfile. Default:", pidfile
    print "  -f, --force             start without serial Device. Default:", force
    print "  -d, --daemonize          Enable daemonize. Default:", daemonize
    print "  -l, --log               Enable debugging. Default:", log
    print "  -u, --usage             print help"


try:
    opts, args = getopt.getopt(sys.argv[1:], "i:p:e:fdlu", 
                              ["serial=", "port=", "pidfile=", "force", "daemonize", "log", "usage"])
    print "debug args:", args
    print "debug opts:", opts
except getopt.GetoptError:          
    print "getopt Error, cant define opts, arg"
    usage()                         
    sys.exit(2)

for opt, arg in opts:
    if opt in ("-i", "--serial"):
        serial = arg
    elif opt in ("-p", "--port"):
        port = int(arg) 
    elif opt in ("-e", "--pidfile"):
        pidfile = str(arg)
    elif opt in ("-f", "--force"):
        force = "true"
    elif opt in ("-d", "--daemonize"):
        daemonize = "true"  
    elif opt in ("-l", "--log"):
        log = "true"  
    elif opt in ("-u", "--usage"):
        usage()
        sys.exit(2)
    else:
        print "unknown option"
        usage()
        sys.exit(2)

if log == "true":
    logging.basicConfig(level=logging.DEBUG, format="%(levelname)s: %(filename)s: %(message)s")
else:
    logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(filename)s: %(message)s")

if daemonize == 'true': 
    daemon()
else:
    MLD().serve(port)
