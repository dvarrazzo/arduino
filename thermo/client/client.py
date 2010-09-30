#!/usr/bin/env python

import sys
import serial
import urllib
from datetime import datetime, timedelta
from urllib2 import urlopen

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
                print "error reading:", e
            else:
                print value
                if ts.minute != minute:
                    minute = ts.minute
                    send_value(opt, ts, value)

def parse_options():
    class O:
        pass

    o = O()
    o.url = sys.argv[1]
    return o

def send_value(opt, ts, value):
    print "send", value
    d = {
        'ts': ts.strftime("%Y-%m-%d %H:%M:%S"),
        'value': str(value) }
    try:
        urllib.urlopen(opt.url, urllib.urlencode(d)).read()
    except Exception, e:
        print "error sending", e

    print >>open("values.log", 'a'), d['ts'], d['value']

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print
        pass
