import os
import time
import muselsl as msl
from multiprocessing import Process, Event
from recorder import record_muse, record_all

def stream(address):
    msl.stream(address)

def view(version):
    msl.view(version=version)

def record_menu(filename):
    # Recording selection
    print("1. Record EEG until stopped")
    print("2. Record EEG for a set time")
    print("3. Record EEG and buttons until stopped")
    record_mode = input("Select recording mode : ")

    stop_event = Event()

    if record_mode == "1" :
        # record until stopped
        record_process = Process(target=record_muse, args=(stop_event, filename))
        record_process.start()
        time.sleep(5) # This is here only to allow the next print to be visible

        # Wait for user input to stop recording
        input("Press Enter to stop recording...")
        stop_event.set()
        record_process.join()
    elif record_mode == "2" :
        # record for a set time
        record_time = input("Enter the time in seconds to record for : ")
        try:
            record_time = int(record_time)
            if record_time <= 0:
                raise ValueError("The recording time must be a positive number.")
        except ValueError as e:
            print(f"Invalid input: {e}, recording aborted")
            return
        record_process = Process(target=record_muse, args=(stop_event,filename, 1))
        record_process.start()
        time.sleep(record_time + 2)
        stop_event.set()
        record_process.join()
    elif record_mode == "3" :
        # record until stopped
        record_process = Process(target=record_muse, args=(stop_event, filename, 1))
        record_process.start()
        time.sleep(5) # This is here only to allow the next print to be visible

        # Wait for user input to stop recording
        input("Press Enter to stop recording...")
        stop_event.set()
        record_process.join()
    else:
        print("Invalid selection, recording aborted")
        return

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

    # Record the data
    record_menu(filename)

    # end all processes
    view_process.terminate()
    stream_process.terminate()

if __name__ == "__main__":
    setup()