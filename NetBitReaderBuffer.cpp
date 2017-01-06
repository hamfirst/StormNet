
#include "NetBitReaderBuffer.h"
#include "NetException.h"

#include <string.h>

NetBitReaderBuffer::NetBitReaderBuffer(void * buffer, std::size_t num_bytes)
  : m_Buffer((uint8_t *)buffer), m_Offset(0), m_NumBytes(num_bytes), m_Bit(0)
{

}

uint64_t NetBitReaderBuffer::ReadUBits(int num_bits)
{
  if (num_bits <= 0)
  {
    return 0;
  }

  if (m_NumBytes <= 0)
  {
    NET_THROW_OR(std::out_of_range("NetBitReader requested too much data"), return 0);
  }

  uint64_t mask_bits = (1ULL << num_bits) - 1ULL;
  uint64_t val = (*m_Buffer) >> m_Bit;
  int read_bits = num_bits;

  if (m_Bit + num_bits >= 8)
  {
    read_bits = 8 - m_Bit;

    m_Bit = 0;
    m_NumBytes--;
    m_Buffer++;
    m_Offset++;
    return (val & mask_bits) | (ReadUBits(num_bits - read_bits) << read_bits);
  }

  m_Bit += num_bits;
  return (val & mask_bits);
}

int64_t NetBitReaderBuffer::ReadSBits(int num_bits)
{
  return SignExtend(ReadUBits(num_bits), num_bits);
}

void NetBitReaderBuffer::ReadBuffer(void * buffer, std::size_t num_bytes)
{
  if (m_Bit == 0 && m_NumBytes >= num_bytes)
  {
    memcpy(buffer, m_Buffer, num_bytes);
    m_Buffer += num_bytes;
    m_Offset += num_bytes;
    m_NumBytes -= num_bytes;
  }
  else
  {
    NetBitReader::ReadBuffer(buffer, num_bytes);
  }
}

