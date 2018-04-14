/*
|| @author         Brett Hagman <brett@oryng.org>
|| @url            http://oryng.org/
|| @contribution   Christopher Baker <https://christopherbaker.net>
||
|| @description
|| |
|| | PackedSerial
|| | Using COBS and a straightforward data packing method, PackedSerial provides a
|| | convenient way to send and receive all kinds of data reliably between devices.
|| |
|| #
||
|| @notes:
|| |
|| | Thanks to Chistopher Baker and his well-documented source code
|| | for PacketSerial, which uses any type of packet algorithm to 
|| | send and receive data.  Please see: https://github.com/bakercp/PacketSerial
|| | This library uses his implementation almost verbatim, but strictly uses
|| | the COBS packet encoding.
|| |
|| #
||
|| @todo:
|| |
|| | Expand the PackedSerial interface to further abstract the building
|| | of messages.
|| |
|| | PackedSerial.buildMessage(bleh)
|| | PackedSerial.sendMessage()
|| | PackedSerial.println(bleh)
|| |
|| #
||
|| @license Please see LICENSE.
||
*/


#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#include <Print.h>
#endif

#include <stdint.h>
#include <string.h>

class SimplePack
{
  public:
    static uint8_t writeUInt8(uint8_t *dest, uint8_t n, uint8_t offset)
    {
      dest[offset] = n;
      return offset + 1;
    }

    static uint8_t writeInt8(uint8_t *dest, int8_t n, uint8_t offset)
    {
      return writeUInt8(dest, n, offset);
    }

    static uint8_t writeUInt16BE(uint8_t *dest, uint16_t n, uint8_t offset)
    {
      dest[offset] = (n & 0xff00) >> 8;
      dest[offset + 1] = n & 0xff;
      return offset + 2;
    }

    static uint8_t writeInt16BE(uint8_t *dest, int16_t n, uint8_t offset)
    {
      return writeUInt16BE(dest, n, offset);
    }

    static uint8_t writeString(uint8_t *dest, char *s, uint8_t offset)
    {
      uint8_t len = strlen(s);

      dest[offset] = len;
      offset += 1;
      strcpy((char *)(dest + offset), s);
      return offset + len;
    }

#ifdef ARDUINO
    static uint8_t writeString_P(uint8_t *dest, const __FlashStringHelper *pstr, uint8_t offset)
    {
      uint8_t len = strlen_P((PGM_P)pstr);

      dest[offset] = len;
      offset += 1;
      strcpy_P(dest + offset, (PGM_P)pstr);
      return offset + len;
    }
#endif

    static uint8_t readUInt8(uint8_t *src, uint8_t offset)
    {
      return src[offset];
    }

    static int8_t readInt8(uint8_t *src, uint8_t offset)
    {
      return (int8_t)src[offset];
    }

    static uint16_t readUInt16BE(uint8_t *src, uint8_t offset)
    {
      uint16_t val;

      val = src[offset] << 8;
      val += src[offset + 1];

      return val;
    }

    static int16_t readInt16BE(uint8_t *src, uint8_t offset)
    {
      return (int16_t)readUInt16BE(src, offset);
    }

    static char *readString(char *dest, uint8_t *src, uint8_t offset)
    {
      uint8_t len = src[offset];
      offset += 1;

      strncpy(dest, (char *)(src + offset), len);
      dest[len] = 0;  // terminate

      return dest;
    }
};


/// \brief A Consistent Overhead Byte Stuffing (COBS) Encoder.
///
/// Consistent Overhead Byte Stuffing (COBS) is an encoding that removes all 0
/// bytes from arbitrary binary data. The encoded data consists only of bytes
/// with values from 0x01 to 0xFF. This is useful for preparing data for
/// transmission over a serial link (RS-232 or RS-485 for example), as the 0
/// byte can be used to unambiguously indicate packet boundaries. COBS also has
/// the advantage of adding very little overhead (at least 1 byte, plus up to an
/// additional byte per 254 bytes of data). For messages smaller than 254 bytes,
/// the overhead is constant.
///
/// \sa http://conferences.sigcomm.org/sigcomm/1997/papers/p062.pdf
/// \sa http://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
/// \sa https://github.com/jacquesf/COBS-Consistent-Overhead-Byte-Stuffing
/// \sa http://www.jacquesf.com/2011/03/consistent-overhead-byte-stuffing
class COBS
{
  public:
    /// \brief Encode a byte buffer with the COBS encoder.
    /// \param buffer A pointer to the unencoded buffer to encode.
    /// \param size  The number of bytes in the \p buffer.
    /// \param encodedBuffer The buffer for the encoded bytes.
    /// \returns The number of bytes written to the \p encodedBuffer.
    /// \warning The encodedBuffer must have at least getEncodedBufferSize() allocated.
    static size_t encode(const uint8_t* buffer,
                         size_t size,
                         uint8_t* encodedBuffer)
    {
      size_t read_index  = 0;
      size_t write_index = 1;
      size_t code_index  = 0;
      uint8_t code       = 1;

      while (read_index < size)
      {
        if (buffer[read_index] == 0)
        {
          encodedBuffer[code_index] = code;
          code = 1;
          code_index = write_index++;
          read_index++;
        }
        else
        {
          encodedBuffer[write_index++] = buffer[read_index++];
          code++;

          if (code == 0xFF)
          {
            encodedBuffer[code_index] = code;
            code = 1;
            code_index = write_index++;
          }
        }
      }

      encodedBuffer[code_index] = code;

      return write_index;
    }


    /// \brief Decode a COBS-encoded buffer.
    /// \param encodedBuffer A pointer to the \p encodedBuffer to decode.
    /// \param size The number of bytes in the \p encodedBuffer.
    /// \param decodedBuffer The target buffer for the decoded bytes.
    /// \returns The number of bytes written to the \p decodedBuffer.
    /// \warning decodedBuffer must have a minimum capacity of size.
    static size_t decode(const uint8_t* encodedBuffer,
                         size_t size,
                         uint8_t* decodedBuffer)
    {
      if (size == 0)
        return 0;

      size_t read_index  = 0;
      size_t write_index = 0;
      uint8_t code       = 0;
      uint8_t i          = 0;

      while (read_index < size)
      {
        code = encodedBuffer[read_index];

        if (read_index + code > size && code != 1)
        {
          return 0;
        }

        read_index++;

        for (i = 1; i < code; i++)
        {
          decodedBuffer[write_index++] = encodedBuffer[read_index++];
        }

        if (code != 0xFF && read_index != size)
        {
          decodedBuffer[write_index++] = '\0';
        }
      }

      return write_index;
    }

    /// \brief Get the maximum encoded buffer size needed for a given unencoded buffer size.
    /// \param unencodedBufferSize The size of the buffer to be encoded.
    /// \returns the maximum size of the required encoded buffer.
    static size_t getEncodedBufferSize(size_t unencodedBufferSize)
    {
      return unencodedBufferSize + unencodedBufferSize / 254 + 1;
    }

};


template<size_t BufferSize = 32>
class PackedSerial
{
  public:
    typedef void (*PacketHandlerFunction)(const uint8_t* buffer, size_t size);

    PackedSerial():
      _receiveBufferIndex(0),
      _stream(nullptr),
      _onPacketFunction(nullptr)
    {
    }

    void setStream(Stream &stream)
    {
      _stream = &stream;
    }

    void update()
    {
      if (_stream == nullptr) return;

      while (_stream->available() > 0)
      {
        uint8_t data = _stream->read();

        if (data == 0)  // COBS packet marker
        {
          if (_onPacketFunction)
          {
            uint8_t _decodeBuffer[_receiveBufferIndex];

            size_t numDecoded = COBS::decode(_receiveBuffer,
                                             _receiveBufferIndex,
                                             _decodeBuffer);

            if (_onPacketFunction)
            {
              _onPacketFunction(_decodeBuffer, numDecoded);
            }
          }

          _receiveBufferIndex = 0;
        }
        else
        {
          if ((_receiveBufferIndex + 1) < BufferSize)
          {
            _receiveBuffer[_receiveBufferIndex++] = data;
          }
          else
          {
            // Error, buffer overflow if we write.
          }
        }
      }
    }

    void send(const uint8_t *buffer, size_t size) const
    {
      if (_stream == nullptr || buffer == nullptr || size == 0) return;

      uint8_t _encodeBuffer[COBS::getEncodedBufferSize(size)];

      size_t numEncoded = COBS::encode(buffer,
                                       size,
                                       _encodeBuffer);

      _stream->write(_encodeBuffer, numEncoded);
      _stream->write((uint8_t)'\0');  // COBS packet marker
    }

    void setPacketHandler(PacketHandlerFunction onPacketFunction)
    {
      _onPacketFunction = onPacketFunction;
    }

  private:
    uint8_t _receiveBuffer[BufferSize];
    size_t _receiveBufferIndex = 0;

    Stream *_stream = nullptr;

    PacketHandlerFunction _onPacketFunction = nullptr;

};

