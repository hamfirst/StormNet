#pragma once

#include "NetTransmitter.h"

class NetClientInterface
{
public:

  virtual void GotConnection(NetTransmitter * transmitter) = 0;
  virtual void ConnectionFailed() = 0;
  virtual void Disconnected() = 0;

  virtual void GotMessage(NetBitReader & reader, bool ack, int channel_index) = 0;
  virtual void GotMessage(NetBitReader & reader) = 0;
};
