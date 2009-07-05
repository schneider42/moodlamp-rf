class IHexFile:
    def __init__(self):
        self.data = []
        self.done = False
        self.adr = -1
        self.pagesize = 256
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
        
 
