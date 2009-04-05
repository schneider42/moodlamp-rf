class Crc8:
    def crc_update(self, crc, data):
        for i in range(8):
            if (crc ^ data) & 1:
                crc = (crc >> 1) ^ 0x8c
            else:
                crc = crc >> 1
    
            data = data >> 1
    
        return crc
    
    def crc_calc(self, data, pagesize):
        crc = 0
        for i in range (pagesize):
            crc = self.crc_update (crc, ord(data[i]))
    
        return crc
