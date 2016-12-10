#pragma once

#include "NetBitWriter.h"
#include "NetBitReader.h"
#include "NetPipeMode.h"


class NetTransmitter
{
public:
  virtual NetBitWriter & CreateWriter(NetPipeMode mode) = 0;
  virtual void SendMessage(NetBitWriter & writer) = 0;
};
