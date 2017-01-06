#pragma once

#include <memory>

#include "NetBitWriter.h"
#include "NetBitReader.h"
#include "NetTransmitter.h"
#include "NetBitUtil.h"
#include "NetMetaUtil.h"

template <typename ... MessagePipeTypes>
class NetProtocolSender
{
public:
  NetProtocolSender(NetTransmitter *transmitter, int opposite_channel_bits) :
    m_OppositeChannelBits(opposite_channel_bits)
  {
    NetPipeMode modes[sizeof...(MessagePipeTypes)] = { MessagePipeTypes::PipeMode... };
    NetPipeMode * mode_ptr = &modes[0];

    auto initializer = [&](auto & ... elems)
    {
      int channel_index = 0;
      InitSender(channel_index, mode_ptr, transmitter, elems...);
    };

    NetMetaUtil::Apply(initializer, m_Senders);
  }

  template<std::size_t ChannelIndex>
  auto & GetChannel()
  {
    return std::get<ChannelIndex>(m_Senders);
  }

  void GotAck(NetBitReader & reader)
  {
    int target_channel = (int)reader.ReadUBits(GetRequiredBits(sizeof...(MessagePipeTypes)-1));

    auto visitor = [&](auto & ... elems)
    {
      int channel_index = 0;
      ProcessAck(channel_index, target_channel, reader, elems...);
    };

    NetMetaUtil::Apply(visitor, m_Senders);
  }

private:

  template <typename SenderType, typename ... Args>
  void InitSender(int & channel_index, NetPipeMode * mode_ptr, NetTransmitter *transmitter, SenderType & sender, Args && ... args)
  {
    sender.Initialize(transmitter, *mode_ptr, channel_index, GetRequiredBits(sizeof...(MessagePipeTypes) - 1));

    channel_index++;
    mode_ptr++;
    InitSender(channel_index, mode_ptr, transmitter, std::forward<Args>(args)...);
  }

  void InitSender(int & channel_index, NetPipeMode * mode_ptr, NetTransmitter *transmitter)
  {

  }

  template <typename SenderType, typename ... Args>
  void ProcessAck(int & channel_index, int & target_index, NetBitReader & reader, SenderType & sender, Args && ... args)
  {
    if (channel_index == target_index)
    {
      sender.GotAck(reader);
      return;
    }
    else
    {
      channel_index++;
      ProcessAck(channel_index, target_index, reader, std::forward<Args>(args)...);
    }
  }

  void ProcessAck(int & channel_index, int & target_index, NetBitReader & reader)
  {

  }

private:

  std::tuple<typename MessagePipeTypes::SenderType...> m_Senders;
  int m_OppositeChannelBits;
};
