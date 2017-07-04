
#include "NetTransmitter.h"

NetBitWriter & NetTransmitter::CreateMessage(NetPipeMode mode, int channel_index, int channel_bits)
{
  return CreateWriter(mode, channel_index, channel_bits, false);
}

void NetTransmitter::SendMessage(NetBitWriter & writer)
{
  SendWriter(writer);
}

NetBitWriter & NetTransmitter::CreateAck(NetPipeMode mode, int channel_index, int channel_bits)
{
  return CreateWriter(mode, channel_index, channel_bits, true);
}

void NetTransmitter::SendAck(NetBitWriter & writer)
{
  SendWriter(writer);
}

void NetTransmitter::GetDefaultMessageInfo(NetBitReader & reader, int sender_channel_bits, int recv_channel_bits, bool & ack, int & channel_index)
{
  ack = reader.ReadUBits(1) != 0;
  if (ack)
  {
    channel_index = (int)reader.ReadUBits(sender_channel_bits);
  }
  else
  {
    channel_index = (int)reader.ReadUBits(recv_channel_bits);
  }
}

void NetTransmitter::CreateDefaultMessage(NetBitWriter & writer, int channel_index, int channel_bits, bool ack)
{
  writer.WriteBits(ack ? 1 : 0, 1);
  writer.WriteBits(channel_index, channel_bits);
}
