import os
import time
import muselsl as msl
from multiprocessing import Process, Event
from muse_record import record_for, record_until

def stream(address):
    msl.stream(address)

def view(version):
    msl.view(version=version)

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
    time.sleep(12) # Wait for the stream to start

    # Start the viewer process
    view_process = Process(target=view, args=(2,))
    view_process.start()
    time.sleep(5) # Wait for the viewer to start

    # record file to the correct folder
    data_folder = os.path.join(os.getcwd(), "data")
    filename = os.path.join(data_folder, "%s_recording_%s.csv" %("EEG",time.strftime('%Y-%m-%d-%H.%M.%S', time.localtime())))

    # Recording selection
    record_mode = input("Enter 1 to record until stopped, 2 to record for a set time: ")

    if record_mode == "1" :
        # record until stopped
        stop_event = Event()
        record_process = Process(target=record_until, args=(stop_event, filename))
        record_process.start()
        time.sleep(5) # Wait for the recording to start

        # Wait for user input to stop recording
        user_input = input("Press Enter to stop recording...")
        stop_event.set()
    else :
        # record for a set time
        record_time = input("Enter the time in seconds to record for : ")
        try:
            record_time = int(record_time)
        except ValueError:
            print("Invalid input, recording aborted")
            view_process.kill()
            stream_process.kill()
            return
        record_process = Process(target=record_for, args=(record_time,filename))
        record_process.start()

    # end all processes
    record_process.join()
    view_process.kill()
    stream_process.kill()

if __name__ == "__main__":
    setup()