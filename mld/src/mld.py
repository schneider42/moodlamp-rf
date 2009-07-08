#from __future__ import with_statement

daemonize = 'false' #true/false

import sys, os
import asyncore
import rf12interface
import Timer
import moodlamp
import mldclient

class MLD:
    def serve(self, port=2324):
        self.ml = moodlamp.MoodlampList()
        self.interfaces = []
        #try:
        self.interfaces.append(rf12interface.RF12Interface("/dev/ttyUSB0",230400,1,2,self))
        #except:
        #    self.interfaces.append(rf12interface.RF12Interface("/dev/ttyS0",115200,2,self))
        self.server = mldclient.ClientServer(port,self.ml,self.interfaces, self)
        #self.interfaces[0].start()
        Timer.Timer(1,self).start()
        
        asyncore.loop()

    def new_lamp(self, lamp):
        self.server.new_lamp_detected(lamp)
        
    def remove_lamp(self, lamp):
        print "deleting lamp " + str(lamp)
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
            adr+=1
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
            reply = "x"+"".join(data[1:])+("\x00%c%c\x01" %(adr,0))

            if broadcast == True:
                adr = 0
            self.interfaces[0].packet( adr, reply, broadcast ,True)
            self.ml.lock.release()
            return
        
        if data[0] == 'I':
            self.remove_lamp(adr)
            name = "".join(data[1:])
            lamp = moodlamp.Moodlamp(self.interfaces[0], self, adr, name)
            self.ml.append(lamp)
            print "len ml:"+str(len(self.ml))
            self.ml.lock.release()
            return;
        
        """if adr == 0:# and broadcast == True:
            lamp = Moodlamp(self.interfaces[0], self);
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
                m.data(data,broadcast);
                unknown = 0;
                break
        if unknown == 1:
            print "resetting lamp", str(adr)
            self.interfaces[0].packet( adr, "r", 0,True)        #force reset for unknown lamp
                                                                #todo start rebinding
        self.ml.lock.release()
        
    def packet_done(self, adr, broadcast):
        #self.ml.lock.acquire()                            #Todo use a queue or something to prevent blocking
        #print "packet done adr="+str(adr)
        #print "len ml:"+str(len(self.ml))
        for m in self.ml:
            #print "iter ml adr="+str(m.address)
            if m.address == adr:
                #print "adr ok"
                m.packet_done(broadcast);
                break
        #self.ml.lock.release()

    def timer(self):
        #with self.lock:
        self.ml.lock.acquire()
        for m in self.ml:
            m.timer -= 1
            if m.timer <= 0:
                if m is not DummyLamp:
                    self.server.lamp_removed(m)
                self.ml.remove(m)
                
        self.ml.lock.release()   
    
    def flash(self, interface, firmware):
        self.ml.lock.acquire()

        self.ml.lock.release()
        return r

def daemon(stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
    try:
	if os.fork() > 0: os._exit(0)
    except OSError, error:
	    print 'fork #1 failed: %d (%s)' % (error.errno, error.strerror)
	    os._exit(1)
    os.chdir('.')
    os.setsid()
    os.umask(0)
    try:
	    pid = os.fork()
	    if pid > 0:
		    print 'mld daemon pid %d\r\n' % pid
		    os._exit(0)
    except OSError, error:
		    print 'fork #2 failed: %d (%s)' % (error.errno, error.strerror)
		    os._exit(1)
    for f in sys.stdout, sys.stderr: f.flush( )
    si = file(stdin, 'r')
    so = file(stdout, 'a+')
    se = file(stderr, 'a+', 0)
    os.dup2(si.fileno( ), sys.stdin.fileno( ))
    os.dup2(so.fileno( ), sys.stdout.fileno( ))
    os.dup2(se.fileno( ), sys.stderr.fileno( ))
    MLD().serve()

if daemonize == 'true': 
    daemon()
else:
    MLD().serve()
