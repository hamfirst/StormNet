#pragma once

#include <memory>

#include "NetBitWriter.h"
#include "NetBitReader.h"
#include "NetTransmitter.h"
#include "NetBitUtil.h"
#include "NetMetaUtil.h"

template <typename ... MessagePipeTypes>
class NetProtocolReceiver
{
public:
  NetProtocolReceiver(NetTransmitter *transmitter, int channel_bits)
  {
    auto initializer = [&](auto & ... elems)
    {
      int channel_index = 0;
      this->InitReceiver(channel_index, transmitter, channel_bits, elems...);
    };

    NetMetaUtil::Apply(initializer, m_Receivers);
  }

  template<std::size_t ChannelIndex>
  auto & GetChannel()
  {
    return std::get<ChannelIndex>(m_Receivers);
  }

  void GotMessage(NetBitReader & reader, int target_channel)
  {
    auto visitor = [&](auto & ... elems)
    {
      int channel_index = 0;
      this->ProcessMessage(channel_index, target_channel, reader, elems...);
    };

    NetMetaUtil::Apply(visitor, m_Receivers);
  }

  void GotMessage(NetBitReader & reader)
  {
    int target_channel = (int)reader.ReadUBits(GetRequiredBits(sizeof...(MessagePipeTypes)-1));
    GotMessage(reader, target_channel);
  }

private:

  template <typename ReceiverType, typename ... Args>
  void InitReceiver(int & channel_index, NetTransmitter *transmitter, int channel_bits, ReceiverType & receiver, Args && ... args)
  {
    receiver.Initialize(transmitter, NetPipeMode::kUnreliableSequenced, channel_index, channel_bits);

    channel_index++;
    InitReceiver(channel_index, transmitter, channel_bits, std::forward<Args>(args)...);
  }

  void InitReceiver(int & channel_index, NetTransmitter *transmitter, int channel_bits)
  {

  }

  template <typename ReceiverType, typename ... Args>
  void ProcessMessage(int & channel_index, int & target_index, NetBitReader & reader, ReceiverType & receiver, Args && ... args)
  {
    if (channel_index == target_index)
    {
      receiver.GotMessage(reader);
      return;
    }
    else
    {
      channel_index++;
      ProcessMessage(channel_index, target_index, reader, std::forward<Args>(args)...);
    }
  }

  void ProcessMessage(int & channel_index, int & target_index, NetBitReader & reader)
  {

  }

private:

  std::tuple<typename MessagePipeTypes::ReceiverType...> m_Receivers;
};
