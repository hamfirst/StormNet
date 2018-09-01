#pragma once

#include "NetBitWriterVector.h"
#include "NetBitReaderBuffer.h"
#include "NetSerialize.h"
#include "NetDeserialize.h"

class NetReflectionBitBufferReader
{
public:

  NetReflectionBitBufferReader(const NetReflectionBitBufferReader & rhs) = default;
  NetReflectionBitBufferReader(NetReflectionBitBufferReader && rhs) = default;

  NetReflectionBitBufferReader & operator = (const NetReflectionBitBufferReader & rhs) = default;
  NetReflectionBitBufferReader & operator = (NetReflectionBitBufferReader && rhs) = default;

  template <typename T>
  bool ReadData(T & t)
  {
    NetDeserializer<T, NetBitReaderBuffer> d;
    d(t, m_DataReader);
    return true;
  }

protected:
  NetReflectionBitBufferReader(NetBitWriterVector & data_writer);

  friend class NetReflectionBitBuffer;

private:

  NetBitReaderBuffer m_DataReader;
};

class NetReflectionBitBuffer
{
public:

  NetReflectionBitBuffer() = default;
  NetReflectionBitBuffer(const NetReflectionBitBuffer & rhs) = default;
  NetReflectionBitBuffer(NetReflectionBitBuffer && rhs) = default;

  NetReflectionBitBuffer & operator = (const NetReflectionBitBuffer & rhs) = default;
  NetReflectionBitBuffer & operator = (NetReflectionBitBuffer && rhs) = default;

  void Reset();
  NetReflectionBitBufferReader CreateReader();

  template <typename T>
  void WriteData(const T & t)
  {
    NetSerializer<T, NetBitWriterVector> s;
    s(t, m_DataWriter);
  }

protected:

  template <typename Type, class NetBitReader>
  friend struct NetDeserializer;

  template <typename Type, class NetBitWriter>
  friend struct NetSerializer;

private:

  NetBitWriterVector m_DataWriter;
};

template <class NetBitWriter>
struct NetSerializer<NetReflectionBitBuffer, NetBitWriter>
{
  void operator()(const NetReflectionBitBuffer & val, NetBitWriter & writer)
  {
    auto bits = val.m_DataWriter.GetTotalBits();
    auto ptr = static_cast<const uint64_t *>(val.m_DataWriter.GetData());

    writer.WriteBits(bits, 16);

    if (bits == 0)
    {
      return;
    }

    while (bits >= 64)
    {
      writer.WriteBits(*ptr, 64);
      ptr++;
      bits -= 64;
    }

    auto u8_ptr = reinterpret_cast<const uint8_t *>(ptr);
    while (bits >= 8)
    {
      writer.WriteBits(*u8_ptr, 8);
      u8_ptr++;
      bits -= 8;
    }

    writer.WriteBits(*u8_ptr, bits);
  }
};

template <class NetBitReader>
struct NetDeserializer<NetReflectionBitBuffer, NetBitReader>
{
  void operator()(NetReflectionBitBuffer & val, NetBitReader & reader)
  {
    auto bits = reader.ReadUBits(16);
    val.Reset();

    if (bits == 0)
    {
      return;
    }

    while (bits >= 64)
    {
      auto v = reader.ReadUBits(64);
      val.m_DataWriter.WriteBits(v, 64);
      bits -= 64;
    }

    while (bits >= 8)
    {
      auto v = reader.ReadUBits(8);
      val.m_DataWriter.WriteBits(v, 8);
      bits -= 8;
    }

    auto v = reader.ReadUBits((int)bits);
    val.m_DataWriter.WriteBits(v, (int)bits);
  }
};
