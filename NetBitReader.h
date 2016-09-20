#pragma once

#include <cstdint>
#include <cstddef>

class NetBitReader
{
public:

  virtual uint64_t ReadUBits(int num_bits) = 0;
  virtual int64_t ReadSBits(int num_bits) = 0;

protected:
  int64_t SignExtend(uint64_t val, int bits);
};
