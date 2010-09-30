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
            temp, rest = s.split("\n", 1)
            buf[:] = [rest]

            try:
                temp = float(temp)
            except Exception, e:
                print "error reading:", e
            else:
                print temp
                if ts.minute != minute:
                    minute = ts.minute
                    send_temp(opt, ts, temp)

def parse_options():
    class O:
        pass

    o = O()
    o.url = sys.argv[1]
    return o

def send_temp(opt, ts, temp):
    print "send", temp
    d = {
        'ts': ts.strftime("%Y-%m-%d %H:%M:%S"),
        'temp': str(temp) }
    try:
        urllib.urlopen(opt.url, urllib.urlencode(d)).read()
    except Exception, e:
        print "error sending", e

    print >>open("temp.log", 'a'), d['ts'], d['temp']

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print
        pass
