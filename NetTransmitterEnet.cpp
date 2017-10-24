
#include "NetTransmitterEnet.h"

#include <enet/enet.h>
#include <cstdlib>
#include <cstring>

#undef SendMessage

int g_LagSim = 0;

NetTransmitterEnet::NetTransmitterEnet()
{
  m_LagSim = g_LagSim;
}

NetBitWriter & NetTransmitterEnet::CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack)
{
  m_WriterMode = mode;
  CreateDefaultMessage(m_Writer, channel_index, channel_bits, ack);
  return m_Writer;
}

void NetTransmitterEnet::SendWriter(NetBitWriter & writer)
{
  uint32_t flags;
  switch (m_WriterMode)
  {
  case NetPipeMode::kReliable:
    flags = ENET_PACKET_FLAG_RELIABLE;
    break;
  case NetPipeMode::kUnreliableUnsequenced:
    flags = ENET_PACKET_FLAG_UNSEQUENCED;
    break;
  case NetPipeMode::kUnreliableSequenced:
    flags = 0;
    break;
  }

  if (m_LagSim == 0)
  {
    auto packet = enet_packet_create(m_Writer.GetData(), m_Writer.GetDataSize(), flags);
    enet_peer_send(m_Peer, 0, packet);
  }
  else
  {
    void * buffer = malloc(m_Writer.GetDataSize());
    memcpy(buffer, m_Writer.GetData(), m_Writer.GetDataSize());
    auto send_time = std::chrono::system_clock::now() + std::chrono::milliseconds(m_LagSim);

    m_PendingPackets.push(PendingPacket{ send_time, buffer, m_Writer.GetDataSize(), flags });
  }

  m_Writer.Reset();
}

void NetTransmitterEnet::Update()
{
  while (m_PendingPackets.size())
  {
    auto & packet = m_PendingPackets.front();
    if (packet.m_SendTime > std::chrono::system_clock::now())
    {
      break;
    }

    auto enet_packet = enet_packet_create(packet.m_Data, packet.m_Size, packet.m_Flags);
    enet_peer_send(m_Peer, 0, enet_packet);

    free(packet.m_Data);
    m_PendingPackets.pop();
  }
}

void NetTransmitterEnet::Clear()
{
  while (m_PendingPackets.size() > 0)
  {
    free(m_PendingPackets.back().m_Data);
    m_PendingPackets.pop();
  }
}
