import threading
import serial
import string
import curses
import sys
import time

print len(sys.argv)

if len(sys.argv) > 4 and int(sys.argv[4]) < 255 :
    remadr = int(sys.argv[4])
else:
    remadr = 3
print "remadr=%d" % remadr

if len(sys.argv) > 3 and int(sys.argv[3]) < 255 :
    adr = int(sys.argv[3])
else:
    adr = 0
print "adr=%d" % adr

if len(sys.argv) > 2 :
    baud = int(sys.argv[2])
else:
    baud = 115200
print "baud=%d" % baud

if len(sys.argv) > 1:
    rf12 = serial.Serial(sys.argv[1], baud)#, timeout=1)
else:
    rf12 = serial.Serial('/dev/ttyUSB1', baud)#, timeout=1)
#rf12 = open('hey2','w')

ar = []
status = ''
adr = 2

class ReadSerial ( threading.Thread ):
    escaped = False

    def readline(self):                #a escapes, c starts new line, b ends line
        global ar
        data = rf12.read(1)            #read single byte
        #print data
        if data == 'a':
            if self.escaped == False:
                self.escaped = True
                return False
            self.escaped = False
        elif self.escaped==True :
           self.escaped = False
           if data == 'c':
               ar = []
               return False
           elif data == 'b' :
               return True                #new string received
        ar.append(data);
        return False

    def run ( self ):
        global ar
        global status
        while 1:
            if self.readline():
                #print ar
                if len(ar) > 3 and ar[0] == 'P':    
                    #print                  #received packet
                    print "%s Sender=%d Rec=%d Seq=%d Data:" % (time.time(),ord(ar[3]),ord(ar[2]),ord(ar[1]))
                    print ar[4:]
                elif len(ar) > 1 and ar[0] == 'D':                        #debug info
                    #if ar[1] != 'f':
                        #print time.time()
                    #print "%s Debug: %s" % (time.time() ,''.join(ar[1:]))
                    ar[0] = 'D'
                elif len(ar) > 1 and ar[0] == 'E':                        #debug info
                    print "%s -----------Error: %s-----------------" % (time.time() ,''.join(ar[1:]))
                elif len(ar) > 1 and ar[0] == 'S':
                    if ar[1] == 'D':
                        #print "Status: done"
                        status = 'D'
                    elif ar[1] == 'T':
                        #print time.time()
                        print "%s ------------------Status: timeout--------------------" % time.time() 
                        status = 'T'
                    else:
                        print "?: %s" % ar
                else:
                    print "?: %s" % ar

readthread = ReadSerial()
readthread.start()


if adr != 0:
    rf12.write("acA%cab" % adr)

#rf12.write("acA\x01ab")        #set address 3
#rf12.write("acA\x50ab")


while True:
    s = raw_input(">")
    if s == 's':
        rf12.write("acS\x02ab")        #sniff all traffic
    elif s == '!s':
        rf12.write("acS\x00ab")
    elif s == 'exit':
        exit()
    elif s == 'pause':
        rf12.write("acP%c" % remadr)
        rf12.write("S\x02ab")
    elif s == 'bl':
        r = 0;
        g = 254;
        b = 0;
        i = 0;
        rf12.write("acP%c" % remadr)
        s  = ("C%c%c%c" % (r,g,b)).replace('a','aa')
        rf12.write(s)
        rf12.write("ab")
        while status == '':
            i += 1
        status = ''
        while True:
            #r = 10
            rf12.write("acP%c" % remadr)
            s  = ("C%c%c%c" % (r,g,b)).replace('a','aa')
            rf12.write(s)
            rf12.write("ab")
            while status == '':
                i += 1
            status = ''
            #r = 10
            rf12.write("acP%c" % remadr)
            s  = ("C%c%c%c" % (r,g,b)).replace('a','aa')
            rf12.write(s)
            rf12.write("ab")
            while status == '':
                i += 1
            status = ''
    elif s == 'f':            
        #r = 250;
        r = 255
        g = 0
        b = 0
        i = 0
        j= time.time()
        while True:
            #r = 2;
            #g = 3;
            #b = 4;
            remadr = 1
            rf12.write("acB%c" % remadr)
            s  = ("C%c%c%c" % (r,g,b)).replace('a','aa')
            #print "%d %d %d" % (r,g,b)
            rf12.write(s)
            rf12.write("ab")
            #color += 1
            #if color > 255:
            #    color = 0
            if r == 0 and not g == 255:
                b -= 1
                g += 1
            elif g == 0 and not b == 255:
                r -= 1
                b += 1
            elif b == 0 and not r == 255:
                g -= 1
                r += 1
            if(r==0 and g==255 and b==0):
                print "FUll %d" % (time.time()-j)
                j = time.time()
            #j+=1
            #i+=1
            #if i==20:
            #    if r==250:
            #        r=0
            #    else:
            #        r=250
            #    i=0
            #if i==0:
            #    g+=5
            #    if g>=250:
            #        i=1
            #else:
            #    g-=5
            #    if g==0:
            #        i=0
            
            """if i==1:
                r=0
                g=0
                b=0
                i=0;
            else:
                r=250
                g=250
                b=250
                i=1
            time.sleep(0.5)
            """    
            """while status == '':
                i += 1 
            status = ''"""
            
            time.sleep(0.009)
            
    elif s=='r':
        rf12.write("acP%c" % remadr)
        rf12.write("C\xff\x00\x00")
        rf12.write("ab");
    elif s=='g':
        rf12.write("acP%c" % remadr)
        rf12.write("C\x00\xFF\x00")
        rf12.write("ab");
    elif s=='b':
        rf12.write("acP%c" % remadr)
        rf12.write("C\x00\x00\xFF")
        rf12.write("ab");
    else:
        #rf12.write("acP\x03")        #send packet to address 1
        rf12.write("acP%c" % remadr)
        rf12.write(s.replace('a','aa'))
        rf12.write("ab");
    
readthread.join(0);