#!/usr/bin/env python

import sys
import Queue
import serial
import threading
from select import select


def input_read(queue):
    """Read from stdin and put the lines read into the queue *q*."""
    while 1:
        line = sys.stdin.readline()
        if not line:
            break

        queue.put(line, block=False)


def main():
    # data to send to the serial
    q = Queue.Queue()

    # set the serial non-blocking to allow use with select
    ser = serial.Serial('/dev/ttyACM0', 9600)
    ser.nonblocking()

    # receive data from stdin asynchronously
    t = threading.Thread(target=input_read, args=(q,))
    t.setDaemon(True)
    t.start()

    # NOTE: data is sent to the serial every second, not immediately.
    # to send it immediately I should wait on stdin too, but it seems tricky
    # in Python.
    while 1:
        # Check if there is data to read from the serial
        rlist, _, _ = select([ser], [], [], 1)
        if ser in rlist:
            sys.stdout.write(ser.read())

        # If there is a command, send it to the serial.
        try:
            cmd = q.get(block=False)
        except Queue.Empty:
            pass
        else:
            ser.write(cmd)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print
        pass
