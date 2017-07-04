
#include "NetTransmitterNull.h"

NetBitWriter & NetTransmitterNull::CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack)
{
  return m_Writer;
}

void NetTransmitterNull::SendWriter(NetBitWriter & writer)
{
  m_Writer.Reset();
}
