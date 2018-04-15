#|
#| @author         Brett Hagman <bhagman@roguerobotics.com>
#| @url            http://roguerobotics.com/
#| @url            http://oryng.org/
#|
#| @description
#| | PackedStringsTest Python receiver
#| | An example to receive 2 separate strings in a single packet.
#| |
#| #
#|
#| @notes
#| | This script requires the 'netstruct' module.  Netstruct is nearly the
#| | same as the 'struct' module, but also supports variable-length strings.
#| |
#| | pip install netstruct
#| |
#| | Please update the "serialPort.port = '...'" line
#| | below to use your serial port.
#| |
#| | python PackedStringsTest.py
#| |
#| #
#|
#| @license Please see LICENSE.
#|

import serial               # To talk to the serial ports
from cobs import cobs       # Decode packets in COBS format
import time                 # To sleep a bit
import netstruct            # Parse the data

dataBuffer = b''

serialPort = serial.Serial()
serialPort.port = 'COM4'
# serialPort.port = '/dev/ttyACM0'
serialPort.baudrate = 115200

serialPort.open()

while True:
  if serialPort.in_waiting > 0: # if we have some data
    dataBuffer += serialPort.read(serialPort.in_waiting) # add it
    if b'\x00' in dataBuffer: # if we have a packet marker...
      # We have a packet!
      (packet, dataBuffer) = dataBuffer.split(b'\x00', 1) # split it
      print(netstruct.unpack(b'b$b$', cobs.decode(packet))) # decode and unpack
  time.sleep(0.5) # have a snooze
