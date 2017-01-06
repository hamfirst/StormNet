
#include "NetTransmitterReplayStreamWriter.h"

#include <stdlib.h>
#include <algorithm>

NetTrasmitterReplayStreamWriter::NetTrasmitterReplayStreamWriter(NetTransmitterBlock * initial_block) :
  m_Block(initial_block),
  m_BitsRemaining(sizeof(NetTransmitterBlock::m_Buffer) * 8),
  m_Offset(0),
  m_Bit(8)
{

}

void NetTrasmitterReplayStreamWriter::WriteBits(uint64_t val, int num_bits)
{
  if (num_bits <= 0)
  {
    return;
  }

  if (m_BitsRemaining < num_bits)
  {
    WriteBits(val, m_BitsRemaining);

    val >>= m_BitsRemaining;
    if (m_Block->m_Next == nullptr)
    {
      m_Block->m_Next = (NetTransmitterBlock *)malloc(sizeof(NetTransmitterBlock));
      m_Block = m_Block->m_Next;
      m_Block->m_Next = nullptr;
    }

    m_BitsRemaining = sizeof(NetTransmitterBlock::m_Buffer) * 8;
    m_Offset = 0;
    m_Bit = 0;
  }

  uint64_t free_bits = 8 - m_Bit;
  if (free_bits == 0)
  {
    m_Block->m_Buffer[m_Offset] = (uint8_t)val;
    m_Bit = std::min(8, num_bits);
    WriteBits(val >> 8, num_bits - 8);
    return;
  }

  uint64_t free_bit_mask = (1 << free_bits) - 1;
  uint64_t used_bit_mask = (1 << m_Bit) - 1;
  uint64_t masked_bits = val & free_bit_mask;

  m_Block->m_Buffer[m_Offset] = (m_Block->m_Buffer[m_Offset] & (uint8_t)used_bit_mask) | (uint8_t)(masked_bits << m_Bit);
  m_Bit = std::min(8, m_Bit + num_bits);

  WriteBits(val >> free_bits, num_bits - std::min(num_bits, (int)free_bits));
}

void NetTrasmitterReplayStreamWriter::WriteSBits(int64_t val, int num_bits)
{
  WriteBits((uint64_t)val, num_bits);
}

void NetTrasmitterReplayStreamWriter::WriteBits(NetBitWriterCursor & cursor, uint64_t val, int num_bits)
{

}

void NetTrasmitterReplayStreamWriter::WriteSBits(NetBitWriterCursor & cursor, int64_t val, int num_bits)
{

}

NetBitWriterCursor NetTrasmitterReplayStreamWriter::Reserve(int num_bits)
{

}

void NetTrasmitterReplayStreamWriter::RollBack(NetBitWriterCursor & cursor)
{
}

NetTransmitterBlock * NetTrasmitterReplayStreamWriter::GetCurrentBlock()
{
  return m_Block;
}
