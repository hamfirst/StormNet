#pragma once

#include "NetTransmitter.h"
#include "NetPipeMode.h"
#include "NetBitWriterVector.h"

#include <queue>
#include <chrono>

struct _ENetPeer;

struct NetTransmitterEnet : public NetTransmitter
{
  NetTransmitterEnet();

  NetBitWriter & CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack) override;
  void SendWriter(NetBitWriter & writer) override;

  void Update();
  void Clear();

public:
  NetBitWriterVector m_Writer = NetBitWriterVector(1400);
  NetPipeMode m_WriterMode = {};
  _ENetPeer * m_Peer = nullptr;
  std::size_t m_ConnectionId = 0;
  bool m_Disconnected = false;

  int m_LagSim;

  struct PendingPacket
  {
    std::chrono::system_clock::time_point m_SendTime;
    void * m_Data;
    std::size_t m_Size;
    uint32_t m_Flags;
  };

  std::queue<PendingPacket> m_PendingPackets;
};
