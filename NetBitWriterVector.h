#pragma once

#include "NetBitWriter.h"

#include <vector>

class NetBitWriterVector : public NetBitWriter
{
public:
  NetBitWriterVector() = default;
  explicit NetBitWriterVector(int reserve_bytes);

  void WriteBits(uint64_t val, int num_bits);
  void WriteSBits(int64_t val, int num_bits);

  void WriteBits(NetBitWriterCursor & cursor, uint64_t val, int num_bits);
  void WriteSBits(NetBitWriterCursor & cursor, int64_t val, int num_bits);

  void * GetData() { return m_Buffer.data(); }
  std::size_t GetDataSize() { return m_Buffer.size(); }

  NetBitWriterCursor Reserve(int num_bits);
  void RollBack(NetBitWriterCursor & cursor);

private:
  std::vector<uint8_t> m_Buffer;
  int m_Bit = 8;
};



