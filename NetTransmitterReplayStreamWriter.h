#pragma once

#include "NetBitWriter.h"

static const int kNetTransmitterBlockSize = 4096;
struct NetTransmitterBlock
{
  NetTransmitterBlock * m_Next;
  uint8_t m_Buffer[kNetTransmitterBlockSize - sizeof(NetTransmitterBlock *)];
};

class NetTrasmitterReplayStreamWriter : public NetBitWriter
{
public:
  NetTrasmitterReplayStreamWriter(NetTransmitterBlock * initial_block);

  virtual void WriteBits(uint64_t val, int num_bits) override;
  virtual void WriteSBits(int64_t val, int num_bits) override;

  virtual void WriteBits(NetBitWriterCursor & cursor, uint64_t val, int num_bits) override;
  virtual void WriteSBits(NetBitWriterCursor & cursor, int64_t val, int num_bits) override;

  virtual NetBitWriterCursor Reserve(int num_bits) override;
  virtual void RollBack(NetBitWriterCursor & cursor) override;

  NetTransmitterBlock * GetCurrentBlock();

private:
  
  std::size_t m_BitsRemaining;
  std::size_t m_Offset;
  int m_Bit;

  NetTransmitterBlock * m_Block;
};

