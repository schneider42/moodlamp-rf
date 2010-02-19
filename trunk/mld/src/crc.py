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
