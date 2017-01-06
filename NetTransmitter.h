#pragma once

#include "NetBitWriter.h"
#include "NetBitReader.h"
#include "NetPipeMode.h"


class NetTransmitter
{
public:
  NetBitWriter & CreateMessage(NetPipeMode mode, int channel_index, int channel_bits);
  void SendMessage(NetBitWriter & writer);

  NetBitWriter & CreateAck(NetPipeMode mode, int channel_index, int channel_bits);
  void SendAck(NetBitWriter & writer);

private:
  virtual NetBitWriter & CreateWriter(NetPipeMode mode) = 0;
  virtual void SendWriter(NetBitWriter & writer) = 0;
};
