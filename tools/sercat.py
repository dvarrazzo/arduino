#!/usr/bin/env python

import sys
import serial

def main():
    ser = serial.Serial('/dev/ttyUSB0', 19200)
    while 1:
        sys.stdout.write(ser.read())

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print
        pass
