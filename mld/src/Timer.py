import threading
import time

class Timer(threading.Thread):
    def __init__(self, time, callback):
        threading.Thread.__init__(self)
        self.callback = callback
        self.time = time
    def run(self):
        while 1:
            time.sleep(self.time)
            self.callback.timer()
