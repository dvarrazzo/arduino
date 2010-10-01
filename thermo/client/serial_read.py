#!/usr/bin/env python
"""Read from the serial and emit one ts/value per minute.

Log messages on stderr.
"""

import sys
import serial
from datetime import datetime, timedelta

import logging
logging.basicConfig()
logger = logging.getLogger()

def main():
    opt = parse_options()
    buf = []
    ser = serial.Serial('/dev/ttyUSB0', 19200)
    minute = None
    while 1:
        buf.append(ser.read())
        if "\n" in buf:
            ts = datetime.now()
            s = "".join(buf)
            value, rest = s.split("\n", 1)
            buf[:] = [rest]

            try:
                value = float(value)
            except Exception, e:
                logger.error("error reading: %s", e)
            else:
                logger.debug("read value: %s", value)
                if ts.minute != minute:
                    minute = ts.minute
                    send_value(opt, ts, value)

def parse_options():
    return None

def send_value(opt, ts, value):
    logger.debug("sending value: %s", value)
    print ts.strftime("%Y-%m-%d %H:%M:%S"), value

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print
        pass
