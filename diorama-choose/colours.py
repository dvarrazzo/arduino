#!/usr/bin/env python3

import time

import serial
from cobs import cobs

import logging
logger = logging.getLogger()
logging.basicConfig(
    level=logging.DEBUG,
    format='%(levelname)s %(asctime)s %(message)s')


class RGB:
    def __init__(self, *args):
        if len(args) == 1:
            s = args[0]
            if len(s) == 3:
                self.red = int(s[0] * 2, 16)
                self.green = int(s[1] * 2, 16)
                self.blue = int(s[2] * 2, 16)
            elif len(s) == 6:
                self.red = int(s[0:2], 16)
                self.green = int(s[2:4], 16)
                self.blue = int(s[4:6], 16)
        elif len(args) == 3:
            self.red, self.green, self.blue = args

        else:
            raise ValueError("it doesn't make sense: %s" % (args,))

    def __bytes__(self):
        return bytes([int(self.red), int(self.green), int(self.blue)])

    def __repr__(self):
        return "RGB(%s, %s, %s)" % (self.red, self.green, self.blue)


def encode_colours(cols):
    return cobs.encode(b''.join(bytes(c) for c in cols)) + b'\0'


def read_from_serial(ser, l=[]):
    while 1:
        c = ser.read()
        if not c:
            break
        l.append(c)
        if c in b'\r\n':
            s = b''.join(l).decode('ascii', 'replace').rstrip()
            if s:
                logger.debug('from serial: %s', s)
            del l[:]


def main():
    ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=0)

    # cols = [
    #     RGB("F00"),
    #     RGB("0F0"),
    #     RGB("00F"),
    #     RGB("FFF"),
    # ]

    cols = [
        RGB("f00"),
        RGB("0f0"),
        RGB("00f"),
    ]

    for i in range(5):
        logger.info("writing %s leds", len(cols))
        ser.write(encode_colours(cols))
        read_from_serial(ser)
        time.sleep(1)

    day = [RGB('59a8ff'), RGB('1f604a'), RGB('ff8080')]
    ser.write(colours.encode_colours(day))

    dusk = [RGB('371b5b'), RGB('0e242b'), RGB('b28b55')]
    ser.write(colours.encode_colours(dusk))

    night = [RGB('0e0e26'), RGB('000'), RGB('bff')]


def blend(c1, c2, alpha):
    if isinstance(c1, list):
        return [blend(i1, i2, alpha) for i1, i2 in zip(c1, c2)]

    return RGB(
        (1 - alpha) * c1.red + alpha * c2.red,
        (1 - alpha) * c1.green + alpha * c2.green,
        (1 - alpha) * c1.blue + alpha * c2.blue)


def anim(*cols):
    if not cols:
        return
    if len(cols) == 1:
        ser.write(colours.encode_colours(cols))
    elif len(cols) == 2:
        for i in range(11):
            time.sleep(0.5)
            ser.write(colours.encode_colours(
                blend(cols[0], cols[1], float(i)/10)))
    else:
        anim(cols[0], cols[1])
        anim(*cols[1:])


if __name__ == '__main__':
    main()
