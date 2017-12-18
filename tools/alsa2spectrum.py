from time import sleep

import serial
import numpy as np
import alsaaudio as aa

rows = 8

sample_rate = 44100
chunk = 1024    # Use a multiple of [rows]

audio = aa.PCM(type=aa.PCM_CAPTURE, mode=aa.PCM_NORMAL)
audio.setchannels(1)
audio.setrate(sample_rate)
audio.setformat(aa.PCM_FORMAT_S16_LE)
audio.setperiodsize(chunk)


def calculate_levels(data, chunk, sample_rate):
    data = np.fromstring(data, dtype='int16')
    fourier = np.fft.rfft(data)
    fourier = np.delete(fourier, len(fourier) - 1)
    power = np.log10(np.abs(fourier) + 0.000001) ** 2
    power = np.reshape(power, (rows, chunk / rows / 2))
    matrix = np.average(power, axis=1) / 4
    return matrix


def main():
    ser = serial.Serial('/dev/ttyACM0', 38400)

    while True:
        l, data = audio.read()
        audio.pause(1)
        if l:
            matrix = calculate_levels(data, chunk, sample_rate)
            matrix = (matrix * 50).clip(max=255).astype('uint8')
            print matrix
            ser.write(matrix.tostring())

        sleep(0.01)
        audio.pause(0)  # Resume capture


if __name__ == '__main__':
    main()
