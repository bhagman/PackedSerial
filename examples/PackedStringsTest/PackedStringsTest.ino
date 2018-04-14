/*
|| PackedStringsTest - PackedSerial Example Sketch.
||
|| Edit and use the accompanying PackedStringsTest.js (in nodejs).
|| In there, you will need to set the serial port that you are using.
||
|| More notes at the bottom.
*/

#include "PackedSerial.h"

PackedSerial<> mySerial = PackedSerial<>();

void setup()
{
  Serial.begin(115200);
  mySerial.setStream(Serial);
//  mySerial.setPacketHandler(handleReceive);
}

void loop()
{
  uint8_t packet[48];
  uint8_t index = 0;

  index = SimplePack::writeString_P(packet, F("Good Test, eh?"), index);
  index = SimplePack::writeString_P(packet, F("Another string"), index);

  mySerial.send(packet, index);

  delay(1000);
}


/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://oryng.org/
||
|| @description
|| | PackedStringsTest
|| | An example to send 2 separate strings in a single packet.
|| |
|| #
||
|| @notes
|| | 
|| | 
|| | 
|| #
||
|| @license Please see LICENSE.
||
*/

