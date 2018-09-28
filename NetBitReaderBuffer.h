#pragma once

#include "NetBitReader.h"

class NetBitReaderBuffer : public NetBitReader
{
public:
  NetBitReaderBuffer(void * buffer, std::size_t num_bytes);
  NetBitReaderBuffer(const NetBitReaderBuffer & rhs) = default;
  NetBitReaderBuffer(NetBitReaderBuffer && rhs) = default;

  NetBitReaderBuffer & operator = (const NetBitReaderBuffer & rhs) = default;
  NetBitReaderBuffer & operator = (NetBitReaderBuffer && rhs) = default;

  uint64_t ReadUBits(int num_bits) override;
  int64_t ReadSBits(int num_bits) override;

  void ReadBuffer(void * buffer, std::size_t num_bytes) override;
  bool IsEmpty() override;

private:
  uint64_t ReadBitsInternal(int num_bits);


protected:
  uint8_t * m_Buffer;
  std::size_t m_Offset;
  std::size_t m_NumBytes;
  int m_Bit;
  int m_TotalBits;
};

