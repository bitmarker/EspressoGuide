__author__ = 'leonidlezner'

import serial
import time
import re


def at_read(serial, cmd):
    print(cmd)
    ser.write('AT+' + cmd + '?\r')
    payload = ser.readline()
    status = ser.readline()

    if not re.search('OK', status):
        return None

    return payload.strip()

ser = serial.Serial('/dev/cu.usbserial-DA01LAAX', 115200, timeout=2)

time.sleep(4)

ser.flushInput()


ser.write('AT\r')

response = ser.readline()

if not re.search('EspressoGuide\s+v([0-9]+\.[0-9]+\.[0-9]+)', response):
    print('Can not find the EspressoGuide!')
    ser.close()
    exit()

ser.flushInput()

print at_read(ser, 'EMIN')
print at_read(ser, 'EMAX')
print at_read(ser, 'SMIN')
print at_read(ser, 'SMAX')
print at_read(ser, 'CNDL')
print at_read(ser, 'UNKN')

ser.close()