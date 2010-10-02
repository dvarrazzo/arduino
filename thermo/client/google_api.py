#!/usr/bin/env python
"""Read the Google Weather API and emit data in the usual format.

Write a sample every 5 minutes on stdout, log on stderr.
"""

import sys
import time
from urllib import quote_plus
from urllib2 import urlopen
from datetime import datetime, timedelta
from xml.etree import cElementTree as ET

import logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger()

def main():
    location = sys.argv[1]
    url = ("http://www.google.com/ig/api?weather="
        + quote_plus(location))

    while 1:
        ts = datetime.now()
        try:
            et = ET.parse(urlopen(url))
            value = float(et.getroot()
                .find('weather')
                .find('current_conditions')
                .find('temp_c')
                .attrib['data'])
        except Exception, e:
            logger.error("error reading: %s", e)
        else:
            print ts.strftime("%Y-%m-%d %H:%M:%S"), value
            time.sleep(5 * 60)

if __name__ == '__main__':
    sys.exit(main())

