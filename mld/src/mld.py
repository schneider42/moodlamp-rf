#from __future__ import with_statement

import socket, sys, threading, time
import asyncore, asynchat
import rf12interface
import traceback
import random
import pickle
import Timer
            
class MoodlampList(list):
    lock = threading.RLock()
    
    def getLamp(self, lamp):
        found = False
        try:
            lamp = int(lamp)
            for l in self:
                if l.address == lamp and l.ready:
                    return l
        except ValueError:
            for l in self:
                if l.name == lamp and l.ready:
                    return l
        raise NotFound()

class IHexFile:
    def __init__(self):
        self.data = []
        self.done = False
        self.adr = -1
        self.pagesize = 128
        pass
    
    def parseLine(self, line):
        if not line[0] == ':':
            return False
        try:
            l = int("0x%s" % line[1:1+2],16)
            adr = int("0x%s" % line[3:3+4],16)
            type = int("0x%s" % line[7:7+2],16)
            
            if type == 1:
                self.done = True
                return True
            if not type == 0:
                return False
            if self.adr + len(self.data) != adr and self.adr != -1:
                return False
            
            if self.adr == -1:
                self.adr = adr
            
            for i in range(l):
                v = chr(int("0x%s" % line[9+i*2:9+i*2+2],16))
                self.data.append(v)
            
        except (IndexError, ValueError):
            return False
        return True
    
    def isDone(self):
        return self.done
    
    def pages(self):
        n =  int(len(self.data) / self.pagesize)
        if n * self.pagesize < len(self.data):
            return n+1
        else:
            return n
    
    def page(self, i):
        if i == self.pages()-1:
            data = self.data[i*self.pagesize:]
            print "data=",data
            data += [chr(0xff)]*(self.pagesize-len(data))
            print "data=", data
            return data
        else:
            return self.data[i*self.pagesize:(i+1)*self.pagesize]
        
        
class DummyLamp:
    timer = 60
    address = 0
    version = "dummy"
    name = "dummy"
    ready = False
    
    def data(self, data, broadcast):
        pass
    
class Moodlamp:
    version = ''
    color = [0xff,0,0]
    done = True
    ready = False
    
    def __init__(self, interface, mld, adr, name):
        self.interface = interface
        self.mld = mld
        self.timer = 60
        self.address = adr
        self.get_version()
        self.state = 2
        self.name = name;
    
    def timer(self):
        self.tick(2, [], False)
        
    def get_address(self):
        pass
        
    def set_address(self, oldadr, newadr):
        self.mld.remove_lamp(newadr)
        self.address = newadr
        self.interface.packet(oldadr, "ADR="+chr(newadr)+self.name, 0, True)
    
    def setcolor(self, color):
        self.color = color
        self.interface.packet( self.address, "C%c%c%c" % (color[0],color[1],color[2]),0,True)
        
    def pause(self, pause):
        self.interface.packet( self.address, "\x17", 0,True)
        
    def updatefirmware(self, firmware):
        self.interface.packet( self.address,"R",0,True);
        chunkno = 0
        chunk = handle.read (pagesize)
        chunklen = len (chunk)
        while 1:
            while len (chunk) < pagesize:
                chunk = chunk + "\377"
            print "%02x (%02x): " % (chunkno, chunklen)
            #self.interface.writeflashpage(firmware)
            
    def tick(self, type, data, broadcast):
        print "ml tick"
        print list(data)
        if self.state == 2:
            if type == 1:
                if len(data) > 2 :
                    if data[0:2] == "D=":
                        print "processing date"
                        self.version = data[2:]
                        self.interface.packet( self.address, "O", 0,True)
                        self.mld.new_lamp(self)
                        self.state = 3
                        self.ready = True
        elif self.state == 3:
            if len(data) > 1:
                if data[0] == 'N':
                    self.name = "".join(data[1:])
            pass
        
    def data(self, data, broadcast):
        #print "ml data:",data
        self.tick(1, data, broadcast)
        self.timer = 60
    
    def packet_done(self, broadcast):
        #print "ml done"
        self.tick(0, [], broadcast)
        
    def setraw(self, mode):
        self.interface.set_raw(mode)
        
    def setprog(self, prog):
        self.interface.packet( self.address, chr(prog), 0,True)
    
    def setname(self, name):
        self.interface.packet( self.address, "N"+name+"\x00", 0,True)

    def get_version(self):
        self.interface.packet( self.address, "V", 0,True)

    def reset(self):
        self.interface.packet( self.address, "r", 0,True)

class NotFound(Exception):
    pass

class MLClient(asynchat.async_chat):
    lock = threading.Lock()
    
    def sendLampNotFound(self):
        self.push("402 No such moodlamp\r\n")
    def sendOK(self):
        self.push("106 OK\r\n")
    def sendNoLampsDetected(self):
        self.push("403 No moodlamps detected\r\n")
    def sendLamp(self, l):
        self.push("101 moodlamp at %d version %s name %s\r\n" % (l.address, l.version,l.name))
    def sendNoSuchInterface(self):
        self.push("405 no such interface")
    def sendInterface(self,i,pos):
        self.push("107 interface %s at %d\r\n" %(i,pos))
    
    def formatExceptionInfo(self, maxTBlevel=5):
         cla, exc, trbk = sys.exc_info()
         excName = cla.__name__
         try:
             excArgs = exc.__dict__["args"]
         except KeyError:
             excArgs = "<no args>"
         excTb = traceback.format_tb(trbk, maxTBlevel)
         return (excName, excArgs, excTb)


    def __init__(self, sock, addr, ml, mld, interfaces):
        asynchat.async_chat.__init__(self, sock)
        self.set_terminator("\r\n")
        self.data = ""
        self.ml = ml
        self.addr = addr
        self.state = 0
        self.push(">")
        self.mld = mld
        self.interfaces = interfaces

    def collect_incoming_data(self, data):
        self.data = self.data + data

    def found_terminator(self):
        print "Client at",self.addr,":",tuple(self.data)
        if self.data.strip() == "":
            if self.state == 0:
                self.push(">")
            return
        s = self.data.strip().split()
        self.ml.lock.acquire()
        try:
            if self.state == 0:
                cmd = s[0]
                print "cmd=",cmd
                ok = True
                
                if cmd == "001":
                    self.push("100 Hello World\r\n")
                    ok = False
                elif cmd == "002":
                    found = 0
                    for m in self.ml:
                        if m.ready:
                            self.sendLamp(m)
                            found+=1
                    if found==0:
                        self.sendNoLampsDetected()
                        ok = False
                elif cmd == "003":
                    m = self.ml.getLamp(s[1])
                    m.setcolor([int(s[2],16),int(s[3],16),int(s[4],16)])
                elif cmd == "004":
                    m = self.ml.getLamp(s[1])
                    m.pause(True);
                elif cmd == "005":
                    pass
                elif cmd == "006":
                    for m in self.interfaces:
                        m.set_raw(True)
                elif cmd == "007":
                    self.firmware = IHexFile()
                    self.state = 1
                elif cmd == "008":
                   self.push("007 start 0X%X len 0X%X\r\n" % (self.firmware.adr, len(self.firmware.data) ))
                elif cmd == "009":
                    if len(self.interfaces) > int(s[1]):
                        interface = self.interfaces[int(s[1])]
                        self.push( "108 flashing on interface %s 0x%X bytes from adr 0x%X\r\n" % 
                                   (interface, len(self.firmware.data), self.firmware.adr))
                        r = interface.flash(self.firmware)
                        if r:
                            self.push("404 error while flashing")
                            ok = False
                    else:
                        self.sendNoSuchInterface()
                        ok = False
                elif cmd == "r":
                    self.interfaces[0].reset()
                elif cmd == "010":
                    pos = 0
                    for i in self.interfaces:
                        self.sendInterface(i, pos)
                        pos+=1
                elif cmd == "011":
                    for i in self.interfaces:
                        i.set_raw(False)
                elif cmd == "012":
                    for i in self.interfaces:
                        i.start_app()
                elif cmd == "013":
                    m = self.ml.getLamp(s[1])
                    m.reset();
                elif cmd == "014":
                    m = self.ml.getLamp(s[1])
                    m.setprog(int(s[2]))
                elif cmd == "015":
                    m = self.ml.getLamp(s[1])
                    m.setname("".join(s[2:]))
                elif cmd == "016":
                    m = self.ml.getLamp(s[1])
                elif cmd == "":
                    pass
                else:
                    self.push("401 unknown command\r\n")
                if ok:
                    self.sendOK()
            elif self.state == 1:
                r = self.firmware.parseLine(self.data)
                if self.firmware.isDone():
                    self.push("105 done\r\n")
                    self.state = 0
                elif r:
                    self.sendOK()
                else:
                    self.push("403 parse failed\r\n")
                    self.state = 0
        except (IndexError, ValueError), err:
            self.push("400 syntax error\r\n")
            print err
            print err.args
            print self.formatExceptionInfo()
        except NotFound, err:
            self.sendLampNotFound()
        if self.state == 0:
            self.push(">")
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
        print "104 Lamp %d removed(timeout)\r\n" % (lamp.address)
        self.ml.lock.release()
        
class ClientServer(asyncore.dispatcher):
    def __init__(self, port,ml,interfaces,mld):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_reuse_addr();
        self.bind(("", port))
        self.listen(5)
        print "serving at port", port, "..."
        self.ml = ml
        self.interfaces = interfaces
        self.clients = []
        self.mld = mld
        
    def handle_accept(self):
        conn, addr = self.accept()
        print 'client is at', addr
        self.clients.append(MLClient(conn, addr,self.ml,self.mld, self.interfaces))
        
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
    def serve(self, port=2324):
        self.ml = MoodlampList()
        self.interfaces = []
        #try:
        self.interfaces.append(rf12interface.RF12Interface("/dev/ttyUSB0",230400,1,2,self))
        #except:
        #    self.interfaces.append(rf12interface.RF12Interface("/dev/ttyS0",115200,2,self))
        self.server = ClientServer(port,self.ml,self.interfaces, self)
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
            dummy = DummyLamp()
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
            lamp = Moodlamp(self.interfaces[0], self, adr, name)
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
    
MLD().serve()
