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


import threading        #for RLock 
import logging

class MoodlampList(list):
    lock = threading.RLock()
    def getLamp(self, lamp):
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
    color = [0xff, 0, 0]
    done = True
    ready = False
    
    def __init__(self, interface, mld, adr, name):
        self.interface = interface
        self.mld = mld
        self.timer = 60
        self.address = adr
        self.get_version()
        self.state = 2
        self.name = name
    
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
        self.interface.packet( self.address, "C%c%c%c" % (color[0], color[1], color[2]), 0, True)
        
    def pause(self, pause):
        self.interface.packet( self.address, "\x17", 0, True)
        #unused pause 
        
    def updatefirmware(self, firmware):
        self.interface.packet(self.address, "R", 0, True)
        chunkno = 0
        chunk = handle.read (pagesize)
        chunklen = len (chunk)
        #unused firmware
        #undefined handle and pagesize
        while 1:
            while len (chunk) < pagesize:
                chunk = chunk + "\377"
            logging.info("chunkno: %02x chunklen: %02x", chunkno, chunklen)
            #self.interface.writeflashpage(firmware)
            
    def tick(self, type, data, broadcast):
        logging.debug("ml tick")
        logging.debug("list data: %s", list(data))
        if self.state == 2:
            if type == 1:
                if len(data) > 2 :
                    if data[0:2] == "D=":
                        logging.info("data[0:2] = D - processing date")
                        self.version = data[2:data.find('H=')-1]
                        self.config = data[data.find('H=')+2:]
                        self.interface.packet( self.address, "O", 0, True)
                        self.mld.new_lamp(self)
                        self.state = 3
                        self.ready = True
        elif self.state == 3:
            if len(data) > 1:
                if data[0] == 'N':
                    self.name = "".join(data[1:])
                    logging.info("get N for Name")
                if data[0] == 'V':
                    logging.info("get V voltage right before calculation: %s", data[2:])
                    v = float(data[2:]) / 1024. * 2.56 * 3
                    logging.info("calculated voltage = %f V", v)
 
            pass
        
    def data(self, data, broadcast):
        logging.debug("ml data: %s", data)
        self.tick(1, data, broadcast)
        self.timer = 60
        #unused broadcast
    
    def packet_done(self, broadcast):
        logging.debug("ml done")
        self.tick(0, [], broadcast)
        
    def setraw(self, mode):
        self.interface.set_raw(mode)
        
    def setprog(self, prog):
        self.interface.packet( self.address, "\x21"+chr(prog), 0, True)
    
    def setname(self, name):
        self.interface.packet( self.address, "N"+name+"\x00", 0, True)

    def getvoltage(self):
        self.interface.packet( self.address, "K", 0, True)

    def get_version(self):
        self.interface.packet( self.address, "V", 0, True)

    def reset(self):
        self.interface.packet( self.address, "r", 0, True)

class NotFound(Exception):
    pass


