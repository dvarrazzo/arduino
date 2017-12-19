# simple pulse reading inspired by https://github.com/atomnuker/pa_fft
# fft inspired by https://github.com/koppi/mk/blob/master/linuxcnc/configs/koppi-cnc/alsa-fft.py

import serial
import numpy as np
import pulseaudio.lib_pulseaudio as pa
from ctypes import c_int, c_int16, byref, sizeof

rows = 8
sample_rate = 44100
chunk = 1024    # Use a multiple of [rows]

# pacmd list-sources | grep name:
device = "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor"


def calculate_levels(data, chunk, sample_rate):
    fourier = np.delete(np.fft.rfft(data), -1)
    fourier = np.reshape(fourier, (rows, chunk / rows / 2))
    fourier = np.abs(np.average(fourier, axis=1) / 4)
    return np.log10(fourier + 0.000001).clip(min=0)


def main():
    ser = serial.Serial('/dev/ttyACM0', 38400)

    ss = pa.pa_sample_spec()
    ss.format = pa.PA_SAMPLE_S16LE
    ss.rate = sample_rate
    ss.channels = 1

    cm = pa.pa_channel_map()
    pa.pa_channel_map_init_mono(cm)
    pa_e = c_int()

    s = pa.pa_simple_new(
        None, "alsa2spectrum", pa.PA_STREAM_RECORD, device,
        "record", ss, cm, None, byref(pa_e))
    if not s:
        raise Exception('pa_simple_new() failed: %s' % pa.pa_strerror(pa_e))

    try:
        pa_buf = (c_int16 * chunk)()
        while 1:
            if 0 > pa.pa_simple_read(
                    s, pa_buf, chunk * sizeof(c_int16), byref(pa_e)):
                raise Exception(
                    'pa_simple_new() failed: %s' % pa.pa_strerror(pa_e))

            a = np.ctypeslib.as_array(pa_buf)
            matrix = calculate_levels(a, chunk, sample_rate)
            matrix = (10 * matrix ** 2).clip(max=255).astype('uint8')

            # print matrix
            ser.write(matrix.tostring())

    finally:
        pa.pa_simple_free(s)


if __name__ == '__main__':
    main()