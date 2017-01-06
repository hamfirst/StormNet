
#include "NetTransmitterMemory.h"

NetBitWriter & NetTransmitterMemory::CreateWriter(NetPipeMode mode)
{
  return m_PendingPacket;
}

void NetTransmitterMemory::SendWriter(NetBitWriter & writer)
{
  m_OutgoingPackets.emplace(std::move(m_PendingPacket));
}
