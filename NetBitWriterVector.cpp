
#include "NetBitWriterVector.h"
#include "NetException.h"

#include <algorithm>

NetBitWriterVector::NetBitWriterVector(int reserve_bytes)
  : m_Buffer(reserve_bytes),
  m_Bit(8)
{

}

void NetBitWriterVector::WriteBits(uint64_t val, int num_bits)
{
  if (num_bits <= 0)
  {
    return;
  }

  uint64_t free_bits = 8 - m_Bit;
  if (free_bits == 0)
  {
    m_Buffer.push_back((uint8_t)val);
    m_Bit = std::min(8, num_bits);
    WriteBits(val >> 8, num_bits - 8);
    return;
  }

  uint64_t free_bit_mask = (1 << free_bits) - 1;
  uint64_t used_bit_mask = (1 << m_Bit) - 1;
  uint64_t masked_bits = val & free_bit_mask;

  m_Buffer.back() = (m_Buffer.back() & (uint8_t)used_bit_mask) | (uint8_t)(masked_bits << m_Bit);
  m_Bit = std::min(8, m_Bit + num_bits);

  WriteBits(val >> free_bits, num_bits - std::min(num_bits, (int)free_bits));
}

void NetBitWriterVector::WriteSBits(int64_t val, int num_bits)
{
  WriteBits((uint64_t)val, num_bits);
}

void NetBitWriterVector::WriteBits(NetBitWriterCursor & cursor, uint64_t val, int num_bits)
{
  if (num_bits <= 0)
  {
    return;
  }

  if (num_bits > cursor.m_Size)
  {
    NET_THROW(std::out_of_range("NetBitWriterCursor overflow"));
  }

  auto buffer = (std::vector<uint8_t> *)cursor.m_Buffer;

  cursor.m_Wrote = true;

  int free_bits = 8 - cursor.m_Bit;

  uint8_t write_bit_mask = (uint8_t)((1ULL << num_bits) - 1);

  if (free_bits == 0)
  {
    uint8_t target_bit_mask = write_bit_mask;
    uint8_t save_bit_mask = ~target_bit_mask;
    uint8_t target_bits = write_bit_mask & val;

    cursor.m_Byte++;
    (*buffer)[cursor.m_Byte] = ((*buffer)[cursor.m_Byte] & save_bit_mask) | target_bits;

    int new_bit = std::min(8, num_bits);
    cursor.m_Size -= new_bit - cursor.m_Bit;

    cursor.m_Bit = new_bit;
    WriteBits(cursor, val >> 8, num_bits - 8);
    return;
  }

  uint8_t target_bit_mask = write_bit_mask << cursor.m_Bit;
  uint8_t target_bits = (write_bit_mask & val) << cursor.m_Bit;
  uint8_t save_bit_mask = ~target_bit_mask;

  (*buffer)[cursor.m_Byte] = ((*buffer)[cursor.m_Byte] & save_bit_mask) | target_bits;
  int new_bit = std::min(8, cursor.m_Bit + num_bits);
  cursor.m_Size -= new_bit - cursor.m_Bit;

  cursor.m_Bit = new_bit;

  WriteBits(cursor, val >> free_bits, num_bits - std::min(num_bits, (int)free_bits));
}

void NetBitWriterVector::WriteSBits(NetBitWriterCursor & cursor, int64_t val, int num_bits)
{
  WriteBits(cursor, (uint64_t)val, num_bits);
}

NetBitWriterCursor NetBitWriterVector::Reserve(int num_bits)
{
  NetBitWriterCursor cursor(this, &m_Buffer, m_Buffer.size() - 1, m_Bit, num_bits);
  WriteBits(0, num_bits);
  return cursor;
}

void NetBitWriterVector::RollBack(NetBitWriterCursor & cursor)
{
  if (cursor.m_Wrote)
  {
    NET_THROW(std::runtime_error("Can't rollback a cursor after it's been written to"));
  }

  m_Buffer.resize(cursor.m_Byte + 1);
  m_Bit = cursor.m_Bit;
}

void NetBitWriterVector::Reset()
{
  m_Buffer.clear();
  m_Bit = 8;
}