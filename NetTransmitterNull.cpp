
#include "NetTransmitterNull.h"

NetBitWriter & NetTransmitterNull::CreateWriter(NetPipeMode mode)
{
  return m_Writer;
}

void NetTransmitterNull::SendWriter(NetBitWriter & writer)
{
  m_Writer.Reset();
}
