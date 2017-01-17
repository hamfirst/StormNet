#pragma once

#include "NetTransmitter.h"
#include "NetBitWriterVector.h"

class NetTransmitterNull : public NetTransmitter
{
private:
  virtual NetBitWriter & CreateWriter(NetPipeMode mode) override;
  virtual void SendWriter(NetBitWriter & writer) override;

private:

  NetBitWriterVector m_Writer;
};
