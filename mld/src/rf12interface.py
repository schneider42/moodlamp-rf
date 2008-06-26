import threading
import serial
import string
import curses
import sys
import time
import math
import random

status = ''
    
class ReadSerial ( threading.Thread ):
    escaped = False
    ar=[]

    def __init__ ( self, rf12, callback):
      self.rf12 = rf12
      threading.Thread.__init__ ( self )
      self.callback = callback
      
    def readline(self):                #a escapes, c starts new line, b ends line
        data = self.rf12.read(1)            #read single byte
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
                #print self.ar

                #elif self.ar[0] == 'S':
                #    if(self.)
                 
                #self.callback.new_packet(''.join(self.ar))
                if len(self.ar) > 3 and self.ar[0] == 'P':    
                    #print                  #received packet
                    print "%s Sender=%d Rec=%d Seq=%d Data:" % (time.time(),ord(self.ar[3]),ord(self.ar[2]),ord(self.ar[1]))
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
                
                if self.callback.mode == 1:
                    self.callback.rawdata(ar)
                    continue
                    
                    
                if self.ar[0] == 'P' or self.ar[0] == 'B':
                    if self.ar[0] == 'P':
                        broadcast = False
                    else:
                        broadcast = True
                    adr = ord(self.ar[3])
                    self.callback.new_packet(adr, ''.join(self.ar[4:]), broadcast)
                    
                elif self.ar[0] == 'E' or self.ar[0] == 'D':
                    type = 't'

class RF12Interface:
    def __init__ ( self, port, baud, adr, callback):
        self.rf12 = serial.Serial(port, baud)#, timeout=1)
        self.adr = adr
        self.readthread = ReadSerial(self.rf12,callback)
        self.rf12.write("acA%cab" % adr)
        self.callback = callback
        self.mode = 0
    
    def start(self):
        self.readthread.start()
        
    def packet(self, remadr, data, broadcast):
        if broadcast == False:
            self.rf12.write("acP%c" % remadr)
        else:
            self.rf12.write("acB%c" % remadr)
        self.rf12.write("acP%c" % remadr)
        self.rf12.write(data.replace('a','aa'))
        self.rf12.write("ab")

    def sniff(self, sniff):
        if sniff == True:
            self.rf12.write("acS\x02ab") 
        else:
            self.rf12.write("acS\x00ab")
            
    def set_mode(self, mode):
        self.rf12.write("acR%cab" % mode)            #raw mode = 1
        self.mode = mode
    
    def rawdata(self, data):
        if data[0] == 'R':
            if data[1] == 0x42:        #page flashed
                pass
        
    def writeflashpage(self, page):

        chunk = page.data[:]
        sendchunk = chunk + chr (crc_calc (chunk))
        sendchunk = chr (0x23) + chr (page.pagenumber) + sendchunk
        self.rf12.write(sendchunk.replace('a','aa'))
        
        
    def crc_update (crc, data):
        for i in range (8):
            if (crc ^ data) & 1:
                crc = (crc >> 1) ^ 0x8c
            else:
                crc = crc >> 1
    
            data = data >> 1
    
        return crc
    
    def crc_calc(data):
        crc = 0
    
        for i in range (pagesize):
            crc = crc_update (crc, ord (data[i]))
    
        return crc
