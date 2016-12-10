#pragma once

#include "NetTransmitter.h"

class NetServerInterface
{
public:
  virtual void GotNewConnection(uint32_t connection_id, uint32_t remote_host, uint16_t remote_port, NetTransmitter * transmitter) = 0;
  virtual void ConnectionLost(uint32_t connection_id) = 0;

  virtual void GotMessage(uint32_t connection_id, NetBitReader & reader) = 0;
};

