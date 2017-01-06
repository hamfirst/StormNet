
#include "NetTransmitter.h"

NetBitWriter & NetTransmitter::CreateMessage(NetPipeMode mode, int channel_index, int channel_bits)
{
  auto & writer = CreateWriter(mode);
  writer.WriteBits(0, 1); // Signal that this is not an ack
  writer.WriteBits(channel_index, channel_bits);
  return writer;
}

void NetTransmitter::SendMessage(NetBitWriter & writer)
{
  SendWriter(writer);
}

NetBitWriter & NetTransmitter::CreateAck(NetPipeMode mode, int channel_index, int channel_bits)
{
  auto & writer = CreateWriter(mode);
  writer.WriteBits(1, 1); // Signal that this is an ack
  writer.WriteBits((uint64_t)channel_index, channel_bits);
  return writer;
}

void NetTransmitter::SendAck(NetBitWriter & writer)
{
  SendWriter(writer);
}
