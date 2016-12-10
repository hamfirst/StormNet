
#include "NetTransmitterMemory.h"

NetBitWriter & NetTransmitterMemory::CreateWriter(NetPipeMode mode)
{
  return m_PendingPacket;
}

void NetTransmitterMemory::SendMessage(NetBitWriter & writer)
{
  m_OutgoingPackets.emplace(std::move(m_PendingPacket));
}
