import os
from pylsl import StreamInlet, resolve_byprop
from time import time, strftime, gmtime
from muselsl.constants import LSL_SCAN_TIMEOUT, LSL_EEG_CHUNK, LSL_PPG_CHUNK, LSL_ACC_CHUNK, LSL_GYRO_CHUNK
from muselsl.record import _save


def record(
    duration: int,
    filename=None,
    save_frequence=5,
    dejitter=False,
    data_source="EEG",
    continuous: bool = True,
) -> None:
    chunk_length = LSL_EEG_CHUNK
    if data_source == "PPG":
        chunk_length = LSL_PPG_CHUNK
    if data_source == "ACC":
        chunk_length = LSL_ACC_CHUNK
    if data_source == "GYRO":
        chunk_length = LSL_GYRO_CHUNK

    if not filename:
        filename = os.path.join(os.getcwd(), "%s_recording_%s.csv" %
                                (data_source,
                                 strftime('%Y-%m-%d-%H.%M.%S', gmtime())))

    print("Looking for a %s stream..." % (data_source))
    streams = resolve_byprop('type', data_source, timeout=LSL_SCAN_TIMEOUT)

    if len(streams) == 0:
        print("Can't find %s stream." % (data_source))
        return

    print("Started acquiring data.")
    inlet = StreamInlet(streams[0], max_chunklen=chunk_length)
    # eeg_time_correction = inlet.time_correction()

    print("Looking for a Markers stream...")
    marker_streams = resolve_byprop(
        'name', 'Markers', timeout=LSL_SCAN_TIMEOUT)

    if marker_streams:
        inlet_marker = StreamInlet(marker_streams[0])
    else:
        inlet_marker = False
        print("Can't find Markers stream.")

    info = inlet.info()
    description = info.desc()

    Nchan = info.channel_count()

    ch = description.child('channels').first_child()
    ch_names = [ch.child_value('label')]
    for i in range(1, Nchan):
        ch = ch.next_sibling()
        ch_names.append(ch.child_value('label'))

    res = []
    timestamps = []
    markers = []
    t_init = time()
    time_correction = inlet.time_correction()
    last_written_timestamp = None
    print('Start recording at time t=%.3f' % t_init)
    print('Time correction: ', time_correction)
    while (time() - t_init) < duration:
        try:
            data, timestamp = inlet.pull_chunk(
                timeout=1.0, max_samples=chunk_length)

            if timestamp:
                res.append(data)
                timestamps.extend(timestamp)
                tr = time()
            if inlet_marker:
                marker, timestamp = inlet_marker.pull_sample(timeout=0.0)
                if timestamp:
                    markers.append([marker, timestamp])

            # Save every save_frequence
            if continuous and (last_written_timestamp is None or last_written_timestamp + save_frequence < timestamps[-1]):
                _save(
                    filename,
                    res,
                    timestamps,
                    time_correction,
                    dejitter,
                    inlet_marker,
                    markers,
                    ch_names,
                    last_written_timestamp=last_written_timestamp,
                )
                last_written_timestamp = timestamps[-1]

        except KeyboardInterrupt:
            break

    time_correction = inlet.time_correction()
    print("Time correction: ", time_correction)

    _save(
        filename,
        res,
        timestamps,
        time_correction,
        dejitter,
        inlet_marker,
        markers,
        ch_names,
    )

    print("Done - wrote file: {}".format(filename))

#TO DO: add record_until method