#pragma once

#include "NetBitReader.h"

class NetBitReaderBuffer : public NetBitReader
{
public:
  NetBitReaderBuffer(void * buffer, std::size_t num_bytes);

  uint64_t ReadUBits(int num_bits);
  int64_t ReadSBits(int num_bits);

  void ReadBuffer(void * buffer, std::size_t num_bytes);

private:
  uint64_t ReadBitsInternal(int num_bits);


protected:
  uint8_t * m_Buffer;
  std::size_t m_Offset;
  std::size_t m_NumBytes;
  int m_Bit;
  int m_TotalBits;
};

