/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://oryng.org/
||
|| @description
|| | PackedStringsTest JavaScript receiver
|| | An example to receive 2 separate strings in a single packet.
|| |
|| #
||
|| @notes
|| | Please update the "const port = new SerialPort('...')" line
|| | below to use your serial port.
|| |
|| | node PackedStringsTest.js
|| |
|| #
||
|| @license Please see LICENSE.
||
*/

const SimplePack = require('./SimplePack.js');
var packer = new SimplePack(['string', 'string']);

const SerialPort = require('serialport');
const cobs = require('cobs');
const Delimiter = SerialPort.parsers.Delimiter;

const port = new SerialPort('COM4', { baudRate: 115200 });
// const port = new SerialPort('/dev/ttyACM0', { baudRate: 115200 });

const parser = port.pipe(new Delimiter({ delimiter: '\0' }));

parser.on('data', data => { console.log(packer.unpack(cobs.decode(data))); } );

