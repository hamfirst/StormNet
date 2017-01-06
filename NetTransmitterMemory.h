#pragma once

#include "NetTransmitter.h"
#include "NetBitWriterVector.h"
#include "NetBitReaderVector.h"

#include <queue>

class NetTransmitterMemory : public NetTransmitter
{
public:
  virtual NetBitWriter & CreateWriter(NetPipeMode mode) override;
  virtual void SendWriter(NetBitWriter & writer) override;

  template <typename Protocol>
  bool SendToProtocol(Protocol & protocol)
  {
    if (m_OutgoingPackets.size() == 0)
    {
      return false;
    }

    while (m_OutgoingPackets.size())
    {
      protocol.GotMessage(m_OutgoingPackets.front());
      m_OutgoingPackets.pop();
    }
    return true;
  }

private:
  NetBitWriterVector m_PendingPacket;
  std::queue<NetBitReaderVector> m_OutgoingPackets;
};
