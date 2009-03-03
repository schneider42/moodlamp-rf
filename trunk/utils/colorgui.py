import gtk
import sys
import socket

host = "10.0.0.23"
port = 2324
addr = (host,port)
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

def tohex(val):
    h = hex(val)[2:]
    if val < 0x10 :
        h = '0'+h
    return h.upper()

lamp = tohex(int(sys.argv[1]))

def new_color(color):
    c = color.get_current_color()
    r = c.red/256;
    g = c.green/256;
    b = c.blue/256;
    cmd = "acC#%s#%s%s%sab"%(lamp,tohex(r),tohex(g),tohex(b))
    s.sendto(cmd,addr)

window = gtk.Window()
window.connect("delete_event", gtk.main_quit)
window.set_border_width(10)
color = gtk.ColorSelection()
color.connect("color_changed",new_color)
window.add(color)
window.show_all()
gtk.main() 
