#pragma once

#include "NetTransmitter.h"
#include "NetBitWriterVector.h"

static const int kNetTransmitterBlockSize = 4096;
static const int kNetTransmitterBufferSize = kNetTransmitterBlockSize - sizeof(void *);

typedef unsigned int(*NetReplayTimerFunc)();

struct NetTransmitterBlock
{
  NetTransmitterBlock * m_Next;
  uint8_t m_Buffer[kNetTransmitterBufferSize];
};


class NetTransmitterReplayStream : public NetTransmitter
{
public:
  NetTransmitterReplayStream(NetReplayTimerFunc time_callback);
  ~NetTransmitterReplayStream();

  virtual NetBitWriter & CreateWriter(NetPipeMode mode) override;
  virtual void SendWriter(NetBitWriter & writer) override;

  void Start();
  bool SaveToFile(const char * file_name);

  void Clear();

private:
  void WriteData(uint8_t * ptr, std::size_t size);

private:

  uint32_t m_StartTime;

  NetBitWriterVector m_Writer;
  NetTransmitterBlock m_InitialBlock;

  NetReplayTimerFunc m_TimeCallback;

  int m_CurrentOffset;
  NetTransmitterBlock * m_CurrentBlock;
};
