import threading
import serial
import string
import sys
import time
import math
import random
import crc
import Queue

status = ''

class Packet:
    data = ""
    adr = -1
    broadcast = False
    def __init__ ( self, data, adr, broadcast):
        self.data = data
        self.adr = adr
        self.broadcast = broadcast

class ProcessPacket ( threading.Thread ):
    def __init__(self, queue, callback):
        threading.Thread.__init__ ( self )
        self.callback = callback
        self.queue = queue
        
    def run(self):
        while True:
            packet = self.queue.get()
            print "processing packet"
            self.callback.new_packet(packet.adr,packet.data, packet.broadcast)
    
class ReadSerial ( threading.Thread ):
    escaped = False
    ar=[]

    def __init__ ( self, rf12, callback, owner, queue):
      self.rf12 = rf12
      threading.Thread.__init__ ( self )
      self.callback = callback
      self.owner = owner
      self.queue = queue
      self.ready = False
      
    def readline(self):                #a escapes, c starts new line, b ends line
        #print '       r'
        data = self.rf12.read(1)            #read single byte
        #print 'd'
        #print data
        if data == 'a':
            if self.escaped == False:
                self.escaped = True
                return False
            self.escaped = False
        elif self.escaped==True :
           self.escaped = False
           if data == 'c':
               self.ar = []
               return False
           elif data == 'b' :
               return True                #new string received
        self.ar.append(data);
        return False

    def run ( self ):
        global status
        while 1:
            if self.readline():
                print "readline", self.ar
                if not self.ready :

                #elif self.ar[0] == 'S':
                #    if(self.)
                 
                #self.callback.new_packet(''.join(self.ar))
                    print "Ignoring:", str(self.ar)
                elif len(self.ar) > 3 and (self.ar[0] == 'P' or self.ar[0] == 'B'):    
                    #print                  #received packet
                    print "%s Sender=%d Rec=%d Data:" % (time.time(),ord(self.ar[3]),ord(self.ar[2]))
                    print self.ar[4:]
                elif len(self.ar) > 1 and self.ar[0] == 'D':                        #debug info
                    #if ar[1] != 'f':
                        #print time.time()
                    #if ar[1] != 'f':
                    print "%s Debug: %s" % (time.time() ,''.join(self.ar[1:]))
                    pass
                elif len(self.ar) > 1 and self.ar[0] == 'E':                        #debug info
                    print "%s -----------Error: %s-----------------" % (time.time() ,''.join(self.ar[1:]))
                elif len(self.ar) > 1 and self.ar[0] == 'S':
                    if self.ar[1] == 'D':
                        print "Status: done"
                        status = 'D'
                    elif self.ar[1] == 'T':
                        #print time.time()
                        print "%s ------------------Status: timeout--------------------" % time.time() 
                        status = 'T'
                    else:
                        print "?: %s" % self.ar
                else:
                    print "?: %s" % self.ar
                
                if self.owner.mode == 1:
                    self.owner.rawdata(self.ar)
                    continue
                    
                if not self.ready or len(self.ar) == 0 :
                    pass    
                elif self.ar[0] == 'P' or self.ar[0] == 'B':
                    if self.ar[0] == 'P':
                        broadcast = False
                    else:
                        broadcast = True
                    adr = ord(self.ar[3])
                    p = Packet(''.join(self.ar[4:]), adr, broadcast)
                    #self.callback.new_packet(adr, ''.join(self.ar[4:]), broadcast)
                    print "queueing packet"
                    self.queue.put(p)                #don't block incomming packet dones
                    
                elif self.ar[0] == 'E' or self.ar[0] == 'D':
                    type = 't'
                elif len(self.ar) > 1 and self.ar[0] == 'S' and self.ar[1] == 'D':
                    self.owner.packet_done()
                elif len(self.ar) > 1 and self.ar[0] == 'S' and self.ar[1] == 'T':
                    self.owner.packet_done()
                elif self.ar[0] == 'I':
                    self.owner.initinterface()
                    

class RF12Interface:
    done = True
    broadcast = 0
    
    def __init__ ( self, port, baud, ownadr, remadr, callback):
        self.rf12 = serial.Serial(port, baud)#, timeout=1)
        self.rf12.flushInput()
        self.rf12.flushOutput()
        self.adr = ownadr
        self.remadr = remadr
        self.queue = Queue.Queue()
        #self.rf12.setTimeout(1)
        #while rf12.read(): pass
        #self.rf12.setTimeout(None)
        self.readthread = ReadSerial(self.rf12,callback, self, self.queue)
        self.packetthread = ProcessPacket(self.queue, callback)
        self.packetthread.start()
        self.mode = 0
        self.readthread.start()
        time.sleep(1)
        self.readthread.ready = True
        print "ready"
        #self.rf12.write("acA%cab" % adr)
        self.write("I%c%c%c"%(ownadr,remadr,remadr))
        self.free = threading.BoundedSemaphore()
        #self.rf12.write("acR%cab" % 0)
        self.callback = callback
        self.set_raw(False)
        self.pageflashed = 0
        self.pagecrc = 0
        #self.free = threading.Event()
        #self.free.set()
        self.done = True
    
    def initinterface(self):
        self.write("I%c%c%c"%(self.adr,self.remadr,0))
        #self.free = threading.BoundedSemaphore()
        self.free.acquire(False);
        self.free.release();
        #if self.free.
        self.set_raw(False)
        self.done = True
        
    def write(self,data):
        #print "write"
        self.rf12.write("ac" + data.replace('a','aa') + "ab")
        
    def packet_done(self):
        self.done = True
        print "release"
        self.free.acquire(False);
        self.free.release()
        self.callback.packet_done(self.broadcast, self.remadr)    #Todo use a queue or something to prevent blocking
        
    def start(self):
        self.readthread.start()
        time.sleep(1)
        self.readthread.ready = True
        print "ready"
        
    def packet(self, remadr, data, broadcast, wait):
        print str(time.time())+" send packet to "+str(remadr)+" len="+str(len(data))
        print list(data)
        if self.mode == 1:
            return 0
        """if self.done == False:
            if wait == False:
                print ":("
                return 1
            while self.done == False:
                time.sleep(0.0001)            #FIXME    use a lock or a message
                pass
        self.done = False"""
        
        #time.sleep(0.1)
        """if not self.free.isSet():
            return 1
        self.free."""
        #print "aquire"
        if not self.free.acquire(wait):
            print "returning badly"
            return 1
        
        #print "sending"
        self.broadcast = broadcast
        self.remadr = remadr
        if broadcast == False:
            self.rf12.write("acP%c" % remadr)
        else:
            self.rf12.write("acB%c" % remadr)
#        self.rf12.write("acP%c" % remadr)#FIXME    use a lock or a message
        self.rf12.write(data.replace('a','aa'))
        self.rf12.write("ab")
        print "sent packet"
        return 0
    
    def sniff(self, sniff):
        if sniff == True:
            print "setting mode to sniffer"
            self.rf12.write("acS\x02ab") 
        else:
            print "sniffer off"
            self.rf12.write("acS\x00ab")
            
    def set_raw(self, mode):
        #while self.done == False:
        #    time.sleep(0.0001)                #FIXME    use a lock or a message
        self.free.acquire()
        if mode == True:
            self.mode = 1
        else:
            self.mode = 0
            #self.done = True
        print "setting raw"
        self.rf12.write("acR%cab" % self.mode)            #raw mode = 1
        self.free.release()
    
    def rawdata(self, data):
        print "rawdata=", data
        if len(data) > 1 and data[0] == 'R':
            if len(data) > 2 and data[1] == chr(0x23) and data[2] == self.pagecrc:        #page flashed
                self.pageflashed = 1
            elif data[1] == 0x42:        #app started
                #self.callback.appstarted()
                pass
    
    def start_app(self):
        print "start app"
        self.rf12.write("acr")
        self.rf12.write("%c%c"%(0x42,0));
        self.rf12.write("ab")
        
    def flashpage(self, data, pagenumber, pagesize):
        #pagesize = 128
        #chunk = data[:]
        print "flashpage"
        print "data=",data
        print "pagenum=",pagenumber
        print "pagesize=",pagesize
        retries = 0
        while True:
            if retries > 10:
                return 1
            sendchunk = []
            sendchunk.append(chr(0x23))
            sendchunk.append(chr(pagenumber))
            sendchunk += data
            crc8 = crc.crc8()
            dcrc = crc8.crc_calc (data, pagesize)
            print "crc=", dcrc
            self.pagecrc = chr(dcrc)
            #sendchunk.append( chr ( crc.crc8.crc_calc (data, pagesize)))
            sendchunk.append( chr(dcrc))
            print "sendchunk=", sendchunk
            file = open("/tmp/file",'w');
            file.write(''.join(data))
            file.close()
            #cmd = 
    
            self.pageflashed = 0
            #self.pagecrc = data[-1]
            self.rf12.write("acr")
            self.rf12.write((''.join(sendchunk)).replace('a','aa'))
            self.rf12.write("ab")
            i = 0
            while not self.pageflashed:
                time.sleep(0.1)
                i+=1
                if i > 5:
                    retries+=1
                    break
            if i <= 5:
                return 0
    
    def flash(self, firmware):
        for i in range(firmware.pages()):
            if self.flashpage(firmware.page(i), i, firmware.pagesize) == 1:
                return 1
        return 0
    
