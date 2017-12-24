# simple pulse reading inspired by https://github.com/atomnuker/pa_fft
# fft inspired by https://github.com/koppi/mk/blob/master/linuxcnc/configs/koppi-cnc/alsa-fft.py

import sys
import serial
import numpy as np
from cobs import cobs
import Queue
import pulseaudio.lib_pulseaudio as pa
from ctypes import cast, sizeof, POINTER, c_int16, c_size_t, c_void_p

import logging
logger = logging.getLogger()
logging.basicConfig(
    level=logging.INFO,
    format='%(levelname)s %(message)s')

rows = 8
sample_rate = 44100
chunk_size = 1024    # Use a multiple of [rows]

# pacmd list-sources | grep name:
# Output mixer
device = "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor"
# Microphone
# device = "alsa_input.pci-0000_00_1b.0.analog-stereo"


class ScriptError(Exception):
    """Controlled exception raised by the script."""


class Recorder(object):
    def __init__(self, source_name, rate):
        self.source_name = source_name
        self.rate = rate
        self._clear_chunk()

        # will become True or False after source_info
        self._source_found = None

        # Wrap callback methods in appropriate ctypefunc instances so
        # that the Pulseaudio C API can call them
        self._context_notify_cb = pa.pa_context_notify_cb_t(self.context_notify_cb)
        self._source_info_cb = pa.pa_source_info_cb_t(self.source_info_cb)
        self._stream_read_cb = pa.pa_stream_request_cb_t(self.stream_read_cb)

        # stream_read_cb() puts finished chunks into this queue
        self._chunks = Queue.Queue()

        # Create the mainloop thread and set our context_notify_cb
        # method to be called when there's updates relating to the
        # connection to Pulseaudio
        _mainloop = pa.pa_threaded_mainloop_new()
        _mainloop_api = pa.pa_threaded_mainloop_get_api(_mainloop)
        context = pa.pa_context_new(_mainloop_api, sys.argv[0])
        pa.pa_context_set_state_callback(context, self._context_notify_cb, None)
        pa.pa_context_connect(context, None, 0, None)
        pa.pa_threaded_mainloop_start(_mainloop)

    def _clear_chunk(self):
        self.chunk = np.empty((chunk_size,), dtype='int16')
        self.ichunk = 0

    def __iter__(self):
        while True:
            try:
                yield self._chunks.get(True, 1)
            except Queue.Empty:
                if self._source_found is None:
                    logger.warn("stream info not received yet")
                    continue
                else:
                    assert self._source_found is False, self._source_found
                    raise ScriptError("source not found: %s" % self.source_name)

    def context_notify_cb(self, context, _):
        state = pa.pa_context_get_state(context)

        if state == pa.PA_CONTEXT_READY:
            logger.info("Pulseaudio connection ready")
            # Connected to Pulseaudio. Now request that source_info_cb
            # be called with information about the available sources.
            o = pa.pa_context_get_source_info_list(context, self._source_info_cb, None)
            pa.pa_operation_unref(o)

        elif state == pa.PA_CONTEXT_FAILED:
            logger.error("Pulseaudio connection failed")

        elif state == pa.PA_CONTEXT_TERMINATED:
            logger.info("Pulseaudio connection terminated")

    def source_info_cb(self, context, source_info_p, _, __):
        if not source_info_p:
            if self._source_found is None:
                self._source_found = False
                logger.warn("source not found: %s", self.source_name)
            return

        source_info = source_info_p.contents
        logger.debug("got source index %s: %s (%s)",
            source_info.index, source_info.name, source_info.description)

        if source_info.name == self.source_name:
            self._source_found = True
            logger.info("recording using source %s", source_info.name)
            samplespec = pa.pa_sample_spec()
            samplespec.channels = 1
            samplespec.format = pa.PA_SAMPLE_S16LE
            samplespec.rate = self.rate

            pa_stream = pa.pa_stream_new(context, sys.argv[0], samplespec, None)
            pa.pa_stream_set_read_callback(
                pa_stream, self._stream_read_cb, source_info.index)
            pa.pa_stream_connect_record(
                pa_stream, source_info.name, None, pa.PA_STREAM_RECORD)

    def stream_read_cb(self, stream, length, index_incr):
        data = c_void_p()
        pa.pa_stream_peek(stream, data, c_size_t(length))
        # no data could mean buffer empty or a hole
        if data:
            data = cast(data, POINTER(c_int16))
            nsamp = length / sizeof(c_int16)
            # a points to data handled by pulse, so it must be copied
            # without leaving this function
            a = np.ctypeslib.as_array(data, (nsamp,))
            si = 0      # pointer in the samples we are working from
            while si < nsamp:
                if nsamp - si + self.ichunk >= chunk_size:
                    # enough samples to finish the chunk
                    sj = min(si + chunk_size - self.ichunk, nsamp)
                    self.chunk[self.ichunk:] = a[si:sj]
                    self._chunks.put(self.chunk)
                    self._clear_chunk()
                    si = sj
                else:
                    # remaining samples, but not enough to finish a chunk
                    jchunk = self.ichunk + nsamp - si
                    self.chunk[self.ichunk:jchunk] = a[si:]
                    self.ichunk = jchunk
                    break

        if length:
            # as per api, pa_stream_drop should not be called if buffer empty
            pa.pa_stream_drop(stream)


def calculate_levels(data, chunk, sample_rate):
    fourier = np.delete(np.fft.rfft(data), -1)
    fourier = np.reshape(fourier, (rows, chunk / rows / 2))
    fourier = np.abs(np.average(fourier, axis=1) / 4)
    return np.log10(fourier + 0.000001).clip(min=0)


def main():
    recorder = Recorder(device, sample_rate)
    ser = serial.Serial('/dev/ttyACM0', 57600)
    for chunk in recorder:
        matrix = calculate_levels(chunk, chunk_size, sample_rate)
        matrix = (10 * matrix ** 2).clip(max=255).astype('uint8')
        print matrix

        packet = cobs.encode(matrix.tostring()) + '\0'
        ser.write(packet)


if __name__ == '__main__':
    try:
        sys.exit(main())

    except ScriptError, e:
        logger.error("%s", e)
        sys.exit(1)

    except Exception:
        logger.exception("unexpected error")
        sys.exit(1)

    except KeyboardInterrupt:
        print
        logger.info("user interrupt")
        sys.exit(1)
