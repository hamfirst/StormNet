#pragma once

#include "NetTransmitter.h"
#include "NetTransmitterReplayStreamWriter.h"

class NetTransmitterReplayStream : public NetTransmitter
{
public:
  virtual NetBitWriter & CreateWriter(NetPipeMode mode) override;
  virtual void SendWriter(NetBitWriter & writer) override;

private:
  NetTrasmitterReplayStreamWriter m_Writer;
  NetTransmitterBlock m_InitialBlock;
};
