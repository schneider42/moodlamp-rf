import socket, sys, time, os
import asyncore, asynchat
import traceback
import moodlamp
import hex
       

class MLClient(asynchat.async_chat):

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
	self.push("? to wake up el presidente\r\n")
        self.push(">")
        self.mld = mld
        self.interfaces = interfaces

    def collect_incoming_data(self, data):
        self.data = self.data + data

    def found_terminator(self):
        #print "Client at",self.addr,":",tuple(self.data)
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
                
		if cmd == "000":
		    self.close() 
                elif cmd == "001":
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
		    if int(s[1]) == 0:
		        for n in self.ml:
			    if n.ready:
				self.push("Send %d -> %s %s %s\r\n" % (n.address, int(s[2],16),int(s[3],16),int(s[4],16)))
			        m = self.ml.getLamp(n.address)
			        m.setcolor([int(s[2],16),int(s[3],16),int(s[4],16)])
		    else:
			self.push("Send %d -> %s %s %s\r\n" % (int(s[1]), int(s[2],16),int(s[3],16),int(s[4],16)))
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
                    self.firmware = hex.IHexFile()
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
                    if int(s[1]) == 0:
		        for n in self.ml:
			    if n.ready:
			        m = self.ml.getLamp(n.address)
			        m.setprog(int(s[2]))
		    else:
                        m = self.ml.getLamp(s[1])
                        m.setprog(int(s[2]))
                elif cmd == "015":
                    m = self.ml.getLamp(s[1])
                    m.setname("".join(s[2:]))
                elif cmd == "016":
                    m = self.ml.getLamp(s[1])
                    m.getvoltage()
		elif cmd == "?":
		    self.push("000 - quit telnet ;)\r\n")
		    self.push("001 - hello world?\r\n")
		    self.push("002 - list of all available and ready moodlamps\r\n")
		    self.push("003 [moodlamp_id] <ff> <00> <00> - fo change color r/g/b as hex\r\n                                   moodlamp id 0 for all moodlamps\r\n")
		    self.push("004 [moodlamp_id] - toggle pause\r\n")
		    self.push("005 - \r\n")
		    self.push("006 - \r\n")
		    self.push("007 - \r\n")
		    self.push("008 - \r\n")
		    self.push("009 - \r\n")
		    self.push("010 - \r\n")
		    self.push("011 - \r\n")
		    self.push("012 - \r\n")
		    self.push("013 [moodlamp_id] - reseting moodlamp\r\n")
		    self.push("014 - \r\n")
		    self.push("015 [moodlamp_id] <name> - to change name\r\n")
		    self.push("016 - \r\n")
		    self.push("r - reseting the serial device\r\n")
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
        except moodlamp.NotFound, err:
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
        self.push("103 New Lamp at %d version %s config %s\r\n" % (lamp.address, lamp.version, lamp.config))
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
