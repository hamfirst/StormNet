
#include "NetTransmitterEnet.h"

#include <enet/enet.h>

#undef SendMessage

NetTransmitterEnet::NetTransmitterEnet()
{

}

NetBitWriter & NetTransmitterEnet::CreateWriter(NetPipeMode mode)
{
  m_WriterMode = mode;
  return m_Writer;
}

void NetTransmitterEnet::SendMessage(NetBitWriter & writer)
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

  auto packet = enet_packet_create(m_Writer.GetData(), m_Writer.GetDataSize(), flags);
  enet_peer_send(m_Peer, 0, packet);
}
