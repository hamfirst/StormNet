#pragma once

#include "NetBitWriter.h"

#include <vector>

class NetBitWriterVector : public NetBitWriter
{
public:
  NetBitWriterVector() = default;
  NetBitWriterVector(const NetBitWriterVector & rhs) = default;
  NetBitWriterVector(NetBitWriterVector && rhs) = default;

  NetBitWriterVector & operator = (const NetBitWriterVector & rhs) = default;
  NetBitWriterVector & operator = (NetBitWriterVector && rhs) = default;

  explicit NetBitWriterVector(int reserve_bytes);

  void WriteBits(uint64_t val, int num_bits);
  void WriteSBits(int64_t val, int num_bits);

  void WriteBits(NetBitWriterCursor & cursor, uint64_t val, int num_bits);
  void WriteSBits(NetBitWriterCursor & cursor, int64_t val, int num_bits);

  void WriteBuffer(void * data, std::size_t num_bytes);

  void * GetData() { return m_Buffer.data(); }
  const void * GetData() const { return m_Buffer.data(); }
  std::size_t GetDataSize() const { return m_Buffer.size(); }

  NetBitWriterCursor Reserve(int num_bits);
  void RollBack(NetBitWriterCursor & cursor);

  void Reset();

  int GetTotalBits() const;

private:

  void WriteBitsInternal(uint64_t val, int num_bits);

private:
  friend class NetBitReaderVector;

  std::vector<uint8_t> m_Buffer;
  int m_Bit = 8;
  int m_TotalBits = 0;
};



