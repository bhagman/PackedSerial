/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://oryng.org/
||
|| @description
|| | SimplePack - A class to easily pack and unpack data types to and from
|| | a Buffer.
|| |
|| #
||
|| @example
|| |
|| | const SimplePack = require('./SimplePack.js');
|| |
|| | var packer = new SimplePack();
|| |
|| | packer.types = ['uint8', 'string', 'uint16'];
|| |
|| | var encoded = packer.pack( [1, 'I like strings', 0x1234] );
|| | console.log(encoded);
|| |
|| | console.log(packer.unpack(encoded));
|| |
|| |
|| #
||
|| @todo
|| |
|| | WHOA! This needs some serious error handling before public consumption. :)
|| |
|| #
||
|| @license Please see LICENSE.
||
*/


class SimplePack {

  constructor(types) {
    this.types = types;
  }

  pack(values) {
    var buf;

    if (values.length === this.types.length) {

      for (var index = 0; index < this.types.length; index++)
      {
        var val;

        switch (this.types[index])
        {
          case 'int8':
            val = new Buffer(1);
            val.writeInt8(values[index]);
            break;
          case 'uint8':
            val = new Buffer(1);
            val.writeUInt8(values[index]);
            break;
          case 'int16':
            val = new Buffer(2);
            val.writeInt16BE(values[index]);
            break;
          case 'uint16':
            val = new Buffer(2);
            val.writeUInt16BE(values[index]);
            break;
          case 'int32':
            val = new Buffer(4);
            val.writeInt32BE(values[index]);
            break;
          case 'uint32':
            val = new Buffer(4);
            val.writeUInt32BE(values[index]);
            break;
          case 'string':
            val = new Buffer(values[index].length + 1);
            val.writeUInt8(values[index].length);
            val.write(values[index], 1);
            break;
          case 'float':
            val = new Buffer(4);
            val.writeFloatBE(values[index], 0);
          default:
            break;
        }

        if (val)
        {
          if (buf)
            buf = Buffer.concat([buf, val]);
          else {
            buf = val;
          }
        }
      }
    }
    else
    {
      // TODO
      // error!
    }

    return buf;
  }

  unpack(buf) {
    // TODO: error handling
    var offset = 0;
    var outArray = new Array();

    for (var index = 0; index < this.types.length; index++)
    {
      var val;

      switch (this.types[index])
      {
        case 'int8':
          val = buf.readInt8(offset);
          offset += 1;
          break;
        case 'uint8':
          val = buf.readUInt8(offset);
          offset += 1;
          break;
        case 'int16':
          val = buf.readInt16BE(offset);
          offset += 2;
          break;
        case 'uint16':
          val = buf.readUInt16BE(offset);
          offset += 2;
          break;
        case 'int32':
          val = buf.readInt32BE(offset);
          offset += 4;
          break;
        case 'uint32':
          val = buf.readUInt32BE(offset);
          offset += 4;
          break;
        case 'string':
          var len = buf.readUInt8(offset);
          offset += 1;
          val = buf.toString('utf8', offset, offset + len);
          offset += len;
          break;
        case 'float':
          val = buf.readFloatBE(offset);
          offset += 4;
          break;
        default:
          break;
      }
      outArray.push(val);
    }

    return outArray;
  }
}

module.exports = SimplePack;

