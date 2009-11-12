import cherrypy
import lamplib
import time
import threading
import thread

while 1:
    lamplib.setcolor("00",0,0,255)
    time.sleep(0.1)
    lamplib.setcolor("00",0,0,0)
    time.sleep(0.1)
    lamplib.setcolor("00",0,0,255)
    time.sleep(0.1)
    lamplib.setcolor("00",0,0,0)
    time.sleep(0.5)

