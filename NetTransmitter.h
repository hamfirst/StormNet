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

  static void GetDefaultMessageInfo(NetBitReader & reader, int sender_channel_bits, int recv_channel_bits, bool & ack, int & channel_index);

protected:
  static void CreateDefaultMessage(NetBitWriter & writer, int channel_index, int channel_bits, bool ack);

private:
  virtual NetBitWriter & CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack) = 0;
  virtual void SendWriter(NetBitWriter & writer) = 0;
};
