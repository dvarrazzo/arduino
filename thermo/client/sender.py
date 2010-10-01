#!/usr/bin/env python
"""Send samples to a web server.

Samples are read from a file in format::

    YYYY-MM-DD HH:MM:SS <VALUE>
"""

import sys
import urllib
from urllib2 import urlopen

import logging
logging.basicConfig()
logger = logging.getLogger()

def main():
    opt = parse_options()
    for line in read_input(opt):
        try:
            rec = parse_line(opt, line)
        except Exception, e:
            logger.error("error reading input: %s", e)
            continue

        try:
            send_sample(opt, rec)
        except Exception, e:
            logger.error("error sending sample: %s", e)
            continue

def read_input(opt):
    if not opt.file_in:
        f = sys.stdin
    else:
        f = open(opt.file_in)

    while 1:
        line = f.readline()
        if not line:
            break
        yield line

def parse_line(opt, line):
    date, time, value = line.split()
    return date + " " + time, value

def send_sample(opt, rec):
    ts, value = rec
    logger.debug("sending %s %s", ts, rec)
    url = opt.server + "/sample/new/"
    d = {'ts': ts, 'value': value, 'device': opt.device}
    resp = urllib.urlopen(url, urllib.urlencode(d))
    if not 200 <= resp.code < 400:
        raise IOError("returned code %d" % resp.code)

def parse_options():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--server")
    parser.add_option("--device")
    parser.add_option("--file-in")

    opt, args = parser.parse_args()
    if not opt.server:
        parser.error("--server currently required")

    if not opt.device:
        parser.error("--device currently required")

    return opt

if __name__ == '__main__':
    sys.exit(main())
