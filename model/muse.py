import os
import time
import muselsl as msl
from multiprocessing import Process

def stream(address):
    msl.stream(address)

def view(version):
    msl.view(version=version)

def record(duration, filename):
    msl.record(duration=duration,filename=filename)

def setup():
    address = ""

    # List available Muses
    muses = msl.list_muses()
    if len(muses) < 1:
        print("No devices found")
        return

    # Get the address of the first Muse
    address = muses[0]["address"]
    print(f"Device address: {address}")

    # Start the stream process
    stream_process = Process(target=stream, args=(address,))
    stream_process.start()

    # Give the stream some time to start
    time.sleep(15)

    # Start the viewer process
    view_process = Process(target=view, args=(2,))
    view_process.start()

    # record file to the correct folder
    data_folder = os.path.join(os.getcwd(), "data")
    filename = os.path.join(data_folder, "%s_recording_%s.csv" %("EEG",time.strftime('%Y-%m-%d-%H.%M.%S', time.localtime())))

    record_process = Process(target=record, args=(20,filename))
    record_process.start()

if __name__ == "__main__":
    setup()