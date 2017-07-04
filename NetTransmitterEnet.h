#pragma once

#include "NetTransmitter.h"
#include "NetPipeMode.h"
#include "NetBitWriterVector.h"

struct _ENetPeer;

struct NetTransmitterEnet : public NetTransmitter
{
  NetTransmitterEnet();

  NetBitWriter & CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack) override;
  void SendWriter(NetBitWriter & writer) override;

public:
  NetBitWriterVector m_Writer = NetBitWriterVector(1400);
  NetPipeMode m_WriterMode = {};
  _ENetPeer * m_Peer = nullptr;
  std::size_t m_ConnectionId = 0;
  bool m_Disconnected = false;
};
