from __future__ import with_statement

import socket, sys, threading, time
import asyncore, asynchat
import rf12interface

class Timer ( threading.Thread ):
    def __init__ ( self, time, callback):
        threading.Thread.__init__ ( self )
        self.callback = callback
        self.time = time
    def run ( self ):
        while 1:
            time.sleep(self.time)
            self.callback.timer()
            
class MoodlampList(list):
    lock = threading.RLock()
    
class Moodlamp:
    version = ''
    color = [0xff,0,0]
    def __init__(self, addr, interface, mld):
        self.interface = interface
        self.address = addr
        self.mld = mld
        self.get_version()
        self.timer = 60
    def setcolor(self, color):
        self.color = color
        self.interface.packet( self.address, "C%c%c%c" % (color[0],color[1],color[2]),0)
        
    def pause(self, pause):
        self.interface.packet( self.address, "S\x02", 0)
        
    def updatefirmware(self, firmware):
        self.interface.packet( self.address,"R",0);
        chunkno = 0
        chunk = handle.read (pagesize)
        chunklen = len (chunk)
        while 1:
            while len (chunk) < pagesize:
                chunk = chunk + "\377"
            print "%02x (%02x): " % (chunkno, chunklen)
            self.interface.writeflashpage(firmware)
        
    def data(self, data):
        print "ml data:",data
        if len(data) > 2 :
            if data[0:2] == "D=":
                self.version = data[2:]
                self.mld.new_lamp(self)
        self.timer = 60

    def get_version(self):
        self.interface.packet( self.address, "V", 0)
        
class MLClient(asynchat.async_chat):
    lock = threading.Lock()

    def __init__(self, sock, addr, ml):
        asynchat.async_chat.__init__(self, sock)
        self.set_terminator("\r\n")
        self.data = ""
        self.ml = ml
        self.addr = addr

    def collect_incoming_data(self, data):
        self.data = self.data + data

    def found_terminator(self):
        print "Client at",self.addr,":",tuple(self.data)
        if self.data.strip() == "":
            return
        s = self.data.strip().split()
        self.ml.lock.acquire()
        try:
            cmd = s[0]
            print "cmd=",cmd
            
            if cmd == "001":
                self.push("100 Hello World\r\n")
            elif cmd == "002":
                for m in self.ml:
                    self.push("101 moodlamp at %d version %s color %s\r\n" % (m.address, m.version,m.color))
                if len(self.ml)==0:
                    self.push("403 No moodlamps detected\r\n")
            elif cmd == "003":
                found = 0
                for m in self.ml:
                    if m.address == int(s[1]):
                        found = 1
                        m.setcolor([int(s[2],16),int(s[3],16),int(s[4],16)])
                if found == 0:
                    self.push("402 No such moodlamp\r\n")
            elif cmd == "004":
                found = 0
                for m in self.ml:
                    if m.address == int(s[1]):
                        found = 1
                        m.pause(True);
                if found == 0:
                    self.push("402 No such moodlamp\r\n")
            elif cmd == "":
                pass
            else:
                self.push("401 unknown command\r\n")
        except (IndexError, ValueError):
            self.push("400 syntax error\r\n")
            
        self.data = ""
        self.ml.lock.release()
        
    def timer(self):
        self.ml.lock.acquire()
        #for m in self.ml:
        #    self.push("moodlamp at %d version %s color %s\r\n" % (m.address, m.version,m.color))
        self.ml.lock.release()
        
    def callback(self, data):
        self.ml.lock.acquire()
        self.push("102 Data: %s\r\n" % data)
        self.ml.lock.release()
        
    def new_lamp_detected(self, lamp):
        self.ml.lock.acquire()
        self.push("103 New Lamp at %d version %s\r\n" % (lamp.address, lamp.version))
        self.ml.lock.release()
        
    def lamp_removed(self, lamp):
        self.ml.lock.acquire()
        self.push("104 Lamp %d removed(timeout)\r\n" % (lamp.address))
        self.ml.lock.release()        
        
class ClientServer(asyncore.dispatcher):
    def __init__(self, port,ml,interface):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_reuse_addr();
        self.bind(("", port))
        self.listen(5)
        print "serving at port", port, "..."
        self.ml = ml
        self.interface = interface
        self.clients = []
        
    def handle_accept(self):
        conn, addr = self.accept()
        print 'client is at', addr
        self.clients.append(MLClient(conn, addr,self.ml))
        
#    def callback(self, data):
        #print data
#        for c in self.clients:
#            c.callback(data)
    def new_lamp_detected(self, lamp):
        for c in self.clients:
            c.new_lamp_detected(lamp)
    def lamp_removed(self, lamp):
        for c in self.clients:
            c.lamp_removed(lamp)
            
class MLD:
    lock = threading.RLock()
    def serve(self, port=2324):
        #self.ml=[]
        self.ml = MoodlampList()
        self.interface = rf12interface.RF12Interface("/dev/ttyUSB0",115200,2,self)
        self.server = ClientServer(port,self.ml,self.interface)
        self.interface.start()
        Timer(1,self).start()
        asyncore.loop()
        
    def new_lamp(self, lamp):
        self.server.new_lamp_detected(lamp)

    def new_packet(self, adr, data, broadcast):
        self.ml.lock.acquire()
        unknown = 1
        for m in self.ml:
            if m.address == adr:
                m.data(data);
                unknown = 0;
                break
        if unknown == 1:
            self.ml.append(Moodlamp(adr, self.interface, self))
        self.ml.lock.release()
        
    def timer(self):
        #with self.lock:
        self.ml.lock.acquire()
        for m in self.ml:
            m.timer -= 1
            if m.timer <= 0:
                self.server.lamp_removed(m)
                self.ml.remove(m)
        self.ml.lock.release()   
        
MLD().serve()