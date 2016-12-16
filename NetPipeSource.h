#pragma once

#include "NetBitWriter.h"

class NetPipeSource
{
public:
  virtual NetBitWriter & CreateAck() = 0;
  virtual void SendAck(NetBitWriter & writer) = 0;
};
