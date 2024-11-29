import muselsl as msl
from multiprocessing import Process
import time

def start_stream(address):
    msl.stream(address)

def start_view(version):
    msl.view(version= version)

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
    stream_process = Process(target=start_stream, args=(address,))
    stream_process.start()

    # Give the stream some time to start
    time.sleep(15)

    # Start the viewer process
    view_process = Process(target=start_view, args=(2,))
    view_process.start()

if __name__ == "__main__":
    setup()
