
#include "NetBitReader.h"

int64_t NetBitReader::SignExtend(uint64_t val, int bits)
{
  uint64_t high_bit = 1ULL << (bits - 1);
  if (high_bit & val)
  {
    uint64_t extended_bits = ~((1ULL << bits) - 1);
    return val | extended_bits;
  }

  return val;
}

void NetBitReader::ReadBuffer(void * buffer, std::size_t num_bytes)
{
  uint64_t * ptr = (uint64_t *)buffer;

  while (num_bytes >= 8)
  {
    *ptr = ReadUBits(64);
    ptr++;
    num_bytes -= 8;
  }

  uint8_t * byte_ptr = (uint8_t *)ptr;
  while (num_bytes > 0)
  {
    *byte_ptr = (uint8_t)ReadUBits(8);
    byte_ptr++;
    num_bytes--;
  }
}
