
#include "NetTransmitterMemory.h"

NetBitWriter & NetTransmitterMemory::CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack)
{
  CreateDefaultMessage(m_PendingPacket, channel_index, channel_bits, ack);
  return m_PendingPacket;
}

void NetTransmitterMemory::SendWriter(NetBitWriter & writer)
{
  m_OutgoingPackets.emplace(std::move(m_PendingPacket));
}
