#!/usr/bin/env python
"""
Read audio from a Pulseaudio source and send a spectrum as packets to Arduino
"""
# Note: you can use
#   pacmd list-sources | grep name:
# to get the list of Pulseaudio sources from command line

import os
import sys
import Queue
from glob import glob
from time import time
from collections import OrderedDict
from ctypes import cast, sizeof, POINTER, c_int16, c_size_t, c_void_p

import serial
import numpy as np
from cobs import cobs
import pulseaudio.lib_pulseaudio as pa

import logging
logger = logging.getLogger()
logging.basicConfig(
    level=logging.INFO,
    format='%(levelname)s %(message)s')

rows = 8
sample_rate = 44100
chunk_size = 1024    # Use a multiple of [rows]


class ScriptError(Exception):
    """Controlled exception raised by the script."""


class Recorder(object):
    def __init__(self, source_name, rate, list_sources=False):
        self.source_name = source_name
        self.rate = rate
        self.sources = OrderedDict()
        self._clear_chunk()
        self._list_sources = list_sources

        # Exception raised in a callback, to be re-raised in the main thread
        self._exception = None

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
                c = self._chunks.get(True, 1)
                if c is None:
                    break
                yield c
            except Queue.Empty:
                if self._exception:
                    raise self._exception

                # Not sure if the following can happen: stay in the loop
                if not self.sources:
                    logger.warn("stream info not received yet")
                else:
                    logger.debug("no sample")

    def context_notify_cb(self, context, _):
        state = pa.pa_context_get_state(context)

        if state == pa.PA_CONTEXT_READY:
            logger.debug("Pulseaudio connection ready")
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
            if self._list_sources:
                # Signal __iter__ to exit
                self._chunks.put(None)
                return

            # All sources listed: pick the best one or the chosen one
            if not self.sources:
                self._exception = ScriptError("no alsa source found")
                return

            if self.source_name:
                try:
                    idx = self.sources[self.source_name]
                except KeyError:
                    self._exception = ScriptError(
                        "alsa source %s not found" % self.source_name)
                    return
            else:
                # Select the first monitor source
                for k in self.sources:
                    if k.endswith('.monitor'):
                        self.source_name = k
                        break
                else:
                    # Whatever, pick the first source
                    self.source_name = self.sources.keys()[0]

                idx = self.sources[self.source_name]

            logger.info("recording using source %s", self.source_name)
            samplespec = pa.pa_sample_spec()
            samplespec.channels = 1
            samplespec.format = pa.PA_SAMPLE_S16LE
            samplespec.rate = self.rate

            pa_stream = pa.pa_stream_new(context, sys.argv[0], samplespec, None)
            pa.pa_stream_set_read_callback(
                pa_stream, self._stream_read_cb, idx)
            pa.pa_stream_connect_record(
                pa_stream, self.source_name, None, pa.PA_STREAM_RECORD)

            return

        source_info = source_info_p.contents
        logger.debug("got source index %s: %s (%s)",
            source_info.index, source_info.name, source_info.description)
        self.sources[source_info.name] = source_info.index

        if self._list_sources:
            print source_info.name, 'index:', source_info.index, \
                source_info.description

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
    # fft inspired by https://github.com/koppi/mk/blob/master/linuxcnc/configs/koppi-cnc/alsa-fft.py
    fourier = np.delete(np.fft.rfft(data), -1)
    fourier = np.reshape(fourier, (rows, chunk / rows / 2))
    fourier = np.abs(np.average(fourier, axis=1) / 4)
    return np.log10(fourier + 0.000001).clip(min=0)


def read_from_serial(ser, l=[]):
    while 1:
        c = ser.read()
        if not c:
            break
        l.append(c)
        if c in '\r\n':
            s = ''.join(l).rstrip()
            if s:
                logger.debug('from serial: %s', s)
            del l[:]


def main():
    opt = parse_cmdline()
    logger.setLevel(opt.loglevel)

    if not opt.serial:
        opt.serial = find_serial()

    logger.info("connecting to Arduino on serial %s", opt.serial)
    try:
        ser = serial.Serial(opt.serial, 57600, timeout=0)
    except Exception, e:
        raise ScriptError(str(e))

    recorder = Recorder(opt.source, sample_rate, opt.list_sources)
    t0 = int(time())
    n = 0
    for chunk in recorder:
        matrix = calculate_levels(chunk, chunk_size, sample_rate)
        matrix = (10 * matrix ** 2).clip(max=255).astype('uint8')
        # print matrix

        packet = cobs.encode(matrix.tostring()) + '\0'
        ser.write(packet)

        t1 = int(time())
        if t1 > t0:
            logger.debug("chunks recorded: %s", n)
            n = 0
            t0 = t1
        n += 1

        read_from_serial(ser)


def find_serial():
    sers = glob('/dev/serial/by-id/*')
    if not sers:
        raise ScriptError(
            "no serial port found in /dev: is Arduino connected?")

    sers.sort()
    asers = [s for s in sers if 'arduino' in s.lower()]
    if asers:
        if len(asers) > 1:
            logger.info(
                "found %s serials looking like Arduino, picking the first",
                len(asers))
        ser = asers[0]
    else:
        logger.info("no serial looks like Arduino; trying one anyway")
        ser = sers[0]

    ser = os.path.abspath(os.path.join(os.path.dirname(ser), os.readlink(ser)))
    return ser


def parse_cmdline():
    from argparse import ArgumentParser
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--serial',
        help="Arduino serial port [default: auto]")
    parser.add_argument('--source',
        help="Pulseaudio source [default: auto]")
    parser.add_argument('--list-sources', action='store_true',
        help="List pulseaudio sources and exit")

    g = parser.add_mutually_exclusive_group()
    g.add_argument('-q', '--quiet', help="Talk less", dest='loglevel',
        action='store_const', const=logging.WARN, default=logging.INFO)
    g.add_argument('-v', '--verbose', help="Talk more", dest='loglevel',
        action='store_const', const=logging.DEBUG, default=logging.INFO)

    opt = parser.parse_args()

    return opt


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
