
#include "NetBitReaderBuffer.h"
#include "NetException.h"

#include <string.h>
#include <assert.h>

NetBitReaderBuffer::NetBitReaderBuffer(void * buffer, std::size_t num_bytes)
  : m_Buffer((uint8_t *)buffer), m_Offset(0), m_NumBytes(num_bytes), m_Bit(0), m_TotalBits(0)
{

}

uint64_t NetBitReaderBuffer::ReadUBits(int num_bits)
{
  m_TotalBits += num_bits;
  return ReadBitsInternal(num_bits);
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
    m_TotalBits += (int)num_bytes * 8;
  }
  else
  {
    NetBitReader::ReadBuffer(buffer, num_bytes);
  }
}

uint64_t NetBitReaderBuffer::ReadBitsInternal(int num_bits)
{
  if (num_bits <= 0)
  {
    return 0;
  }

  if (num_bits == 64 && m_NumBytes > 8)
  {
    uint64_t * ptr = (uint64_t *)m_Buffer;
    uint64_t val = *ptr;

    val >>= m_Bit;
    val |= ((uint64_t)(*(m_Buffer + 8) & ((1 << m_Bit) - 1))) << (64 - m_Bit);

    m_NumBytes -= 8;
    m_Buffer += 8;
    m_Offset += 8;
    return val;
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
    uint64_t final_val = (val & mask_bits) | (ReadBitsInternal(num_bits - read_bits) << read_bits);
    return final_val;
  }

  m_Bit += num_bits;
  return (val & mask_bits);
}
