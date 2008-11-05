#from __future__ import with_statement

import socket, sys, threading, time
import asyncore, asynchat
import rf12interface
import traceback
import random
import pickle

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
    
    def getLamp(self, lamp):
        found = False
        try:
            lamp = int(lamp)
            for l in self:
                if l.address == lamp:
                    found = True
        except ValueError:
            for l in self:
                if l.name == lamp:
                    found = True
        if not found:
            raise NotFound()
        return l

class UUID:
    def getUUID(self):
        u = []
        for i in range(0,16):
            u.append(chr(random.randint(0,255)))
        return tuple(u)

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
        
    
class Moodlamp:
    version = ''
    color = [0xff,0,0]
    done = True
    
    def __init__(self, addr, interface, mld):
        self.interface = interface
        self.address = addr
        self.mld = mld
        self.get_version()
        #self.setup()
        self.timer = 60
        Timer(0.1,self).start()
        
    def __init__(self, interface, mld, adr = None):
        self.interface = interface
        self.mld = mld
        self.timer = 60
        if adr is None:
            self.address = 0
        else:
            self.address = adr
            self.get_version()
            self.state = 2
            self.uuid = 0
        #self.get_version()
        #self.setup(data)
        
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
        
    def setup(self,data):
        self.uuid = []
        if len(data) == 3+16:
            if data[0:3] == "ID=":
                self.uuid = tuple(data[3:])
                adr = self.mld.map_UUID(self.uuid)
                if adr != -1:
                    self.newadr = adr
                    #self.set_address(self.newadr)
                    self.set_address(0,self.newadr)
                    #self.state = 1
                    self.state = 2
                    return True
        elif len(data) == 3:
            if data == "ID?":
                self.uuid = self.mld.get_UUID()
                self.interface.packet(0, "ID="+"".join(self.uuid), 0, True)
                self.state = 0
                return True
        return False
        
    def get_address(self):
        pass
    
    #def set_address(self, adr):
    #    self.interface.packet(0, "ADR="+chr(adr)+"".join(self.uuid), 0, True)
        
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
        
        if self.state == 0:
            if type == 0:
                self.newadr= self.mld.map_UUID(self.uuid)
                #self.mld.remove_lamp(self.newadr)
                #self.set_address(self.newadr)
                #self.state = 1
                self.set_address(0,self.newadr)
                self.state = 2
        elif self.state == 1:
            if type == 0:
                self.address = self.newadr
                self.get_version()
                self.state = 2
        elif self.state == 2:
            if type == 1:
                if len(data) > 2 :
                    if data[0:2] == "D=":
                        print "processing date"
                        self.version = data[2:]
                        self.interface.packet( self.address, "O", 0,True)
                        self.mld.new_lamp(self)
                        self.state = 3
        elif self.state == 3:
            if len(data) > 1:
                if data[0] == 'N':
                    self.name = "".join(data[1:])
            pass
        
    def data(self, data, broadcast):
        #print "ml data:",data
        #if len(data) > 2 :
        #    if data[0:2] == "D=":
        #        self.version = data[2:]
        #        self.mld.new_lamp(self)
        self.tick(1, data, broadcast)
        self.timer = 60
    
    def packet_done(self, broadcast):
        #self.done = True
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
                
                if cmd == "001":
                    self.push("100 Hello World\r\n")
                elif cmd == "002":
                    for m in self.ml:
                        self.push("101 moodlamp at %d version %s name %s\r\n" % (m.address, m.version,m.name))
                    if len(self.ml)==0:
                        self.push("403 No moodlamps detected\r\n")
                    else:
                        self.push("106 OK\r\n")
                elif cmd == "003":
                    found = 0
                    for m in self.ml:
                        if m.address == int(s[1]):
                            found = 1
                            m.setcolor([int(s[2],16),int(s[3],16),int(s[4],16)])
                    if found == 0:
                        self.push("402 No such moodlamp\r\n")
                    self.push("106 OK\r\n")
                elif cmd == "004":
                    found = 0
                    for m in self.ml:
                        if m.address == int(s[1]):
                            found = 1
                            m.pause(True);
                    if found == 0:
                        self.push("402 No such moodlamp\r\n")
                elif cmd == "005":
                    pass
                elif cmd == "006":
                    for m in self.interfaces:
                        m.set_raw(True)
                    self.push("106 OK\r\n")
                elif cmd == "007":
                    self.firmware = IHexFile()
                    self.state = 1
                    self.push("106 OK\r\n")
                elif cmd == "008":
                   self.push("007 start 0X%X len 0X%X\r\n" % (self.firmware.adr, len(self.firmware.data) ))
                elif cmd == "009":
                    if len(self.interfaces) > int(s[1]):
                        #r = self.mld.flash(self.interfaces[int(s[1])], self.firmware)
                        interface = self.interfaces[int(s[1])]
                        self.push( "108 flashing on interface %s 0x%X bytes from adr 0x%X\r\n" % 
                                   (interface, len(self.firmware.data), self.firmware.adr))
                        r = interface.flash(self.firmware)
                        if r == 0:
                            self.push("106 OK\r\n")
                        else:
                            self.push("404 error while flashing")
                    else:
                        self.push("405 no such interface")
                elif cmd == "010":
                    pos = 0
                    for i in self.interfaces:
                        self.push("107 interface %s at %d\r\n" %(i,pos))
                        pos+=1
                    self.push("106 OK\r\n")
                elif cmd == "011":
                    for i in self.interfaces:
                        i.set_raw(False)
                    self.push("106 OK\r\n")
                elif cmd == "012":
                    for i in self.interfaces:
                        i.start_app()
                    self.push("106 OK\r\n")
                elif cmd == "013":
                    found = 0
                    for m in self.ml:
                        if m.address == int(s[1]):
                            found = 1
                            m.reset();
                    if found == 0:
                        self.push("402 No such moodlamp\r\n")
                elif cmd == "014":
                    found = 0
                    for m in self.ml:
                        if m.address == int(s[1]):
                            found = 1
                            m.setprog(int(s[2]))
                    if found == 0:
                        self.push("402 No such moodlamp\r\n")
                elif cmd == "015":
                    found = 0
                    for m in self.ml:
                        if m.address == int(s[1]):
                            found = 1
                            m.setname("".join(s[2:]))
                    if found == 0:
                        self.push("402 No such moodlamp\r\n")
                elif cmd == "016":
                    m = self.ml.getLamp(s[1])
                    
                elif cmd == "":
                    pass
                else:
                    self.push("401 unknown command\r\n")
            elif self.state == 1:
                r = self.firmware.parseLine(self.data)
                if self.firmware.isDone():
                    self.push("105 done\r\n")
                    self.state = 0
                elif r:
                    self.push("106 OK\r\n")
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
    #lock = threading.RLock()
    def serve(self, port=2324):
        #self.ml=[]
        try:
            mapfile = open("moodmap")
            self.map = pickle.load(mapfile)
            print "loaded moodmap:"
            #print self.map
            for key in self.map.keys():
                print "uuid:"+str(key)+" adr:"+str(self.map[key])
                
        except IOError:
            self.map = {}
            print "NO MOODMAP FOUND"
        
        #pickle.load(file)
        self.ml = MoodlampList()
        self.interfaces = []
        #try:
        self.interfaces.append(rf12interface.RF12Interface("/dev/ttyUSB0",230400,1,2,self))
        #lamp = Moodlamp(self.interfaces[0], self,2);
        #self.ml.append(lamp)
        #except:
        #    self.interfaces.append(rf12interface.RF12Interface("/dev/ttyS0",115200,2,self))
        #self.ml.append(Moodlamp(3,self.interfaces[0],self))
        self.server = ClientServer(port,self.ml,self.interfaces, self)
        #self.interfaces[0].start()
        Timer(1,self).start()
        
        asyncore.loop()
        
    def get_UUID(self):
        u = UUID()
        id = u.getUUID()
        while ord(id[0]) == 0:
             id = u.getUUID()
        return id
    
    def map_UUID(self, uuid):
        if self.map.has_key(uuid):
            return self.map[uuid]
        else:
            j=14
            for i in self.map.values():
                if i > j:
                    j = i
            self.map[uuid] = j+1
            mapfile = open("moodmap",'w')
            pickle.dump(self.map, mapfile)
            return j+1
        #return 8                #the debian way
    
    def new_lamp(self, lamp):
        self.server.new_lamp_detected(lamp)
        
    def remove_lamp(self, lamp):
        print "deleting lamp " + str(lamp)
        self.ml.lock.acquire()
        for m in self.ml:
            if m.address == lamp:
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
        
        #    self.ml.append(Moodlamp(self.interfaces[0], self, adr))
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
                self.server.lamp_removed(m)
                self.ml.remove(m)
        self.ml.lock.release()   
    
    def flash(self, interface, firmware):
        self.ml.lock.acquire()

        self.ml.lock.release()
        return r
        #self.interface.
    
MLD().serve()
