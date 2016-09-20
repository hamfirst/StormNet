
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

