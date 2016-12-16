#pragma once

#include "NetBitWriter.h"

class NetPipeSink
{
public:
  virtual NetBitWriter & CreateMessage() = 0;
  virtual void SendMessage(NetBitWriter & writer) = 0;
};
