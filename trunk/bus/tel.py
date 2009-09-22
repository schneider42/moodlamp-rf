import cherrypy
import lamplib
import time
import threading
import thread

class TelIndex(object):
    lock = threading.Lock()

    @cherrypy.expose
    def index(self):
        return ""

    @cherrypy.expose
    def call(self):
        print "call"
        thread.start_new_thread(self.blink, ())
        return "done"
    @cherrypy.expose
    def onhook(self):
        print "onhook"
        #thread.start_new_thread(self.blink, ())
        return "done"

    @cherrypy.expose
    def offhook(self):
        print "offhook"
        #thread.start_new_thread(self.blink, ())
        return "done"

    def blink(self):
        print "getting lock"
        if self.lock.acquire(0):
            print "starting blink"
            for i in range(1,5):
                lamplib.setcolor("00",255,0,0)
                time.sleep(0.1)
                lamplib.setcolor("00",0,0,0)
                time.sleep(0.5)
            lamplib.setcolor("00",0,255,255)
            self.lock.release()
        print "returning from thread"
root = TelIndex()
cherrypy.quickstart(root)

