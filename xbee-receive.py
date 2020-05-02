#! /usr/bin/python

"""
receive_samples.py
By Paul Malmsten, 2010
pmalmsten@gmail.com
This example continuously reads the serial port and processes IO data
received from a remote XBee.
"""


import serial
import binascii

PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

# Open serial port
ser = serial.Serial(PORT, BAUD_RATE)



print "Starting..."
# Continuously read and print packets
while True:
    try: 
        print binascii.hexlify(ser.read(3))

    except KeyboardInterrupt:
        break

ser.close()
