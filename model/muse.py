import muselsl as msl
from threading import Thread
import time

def setup():
    address = ""
    stream = Thread(target=msl.stream, args=(address,))
    view = Thread(target=msl.view)

    muses = msl.list_muses()
    if(len(muses) < 1):
        print("no devices found")
        return
    print(muses[0]["address"]) 
    address = muses[0]["address"]


    stream.run()
    time.sleep(15)
    view.start()

setup()