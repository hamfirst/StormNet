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
  NetProtocolReceiver(NetTransmitter *transmitter)
  {
    auto initializer = [&](auto & ... elems)
    {
      int channel_index = 0;
      InitReceiver(channel_index, transmitter, elems...);
    };

    NetMetaUtil::Apply(initializer, m_Receivers);
  }

  template<std::size_t ChannelIndex>
  auto & GetChannel()
  {
    return *std::get<ChannelIndex>(m_Receivers).get();
  }

  void GotMessage(NetBitReader & reader)
  {
    int target_channel = (int)reader.ReadUBits(GetRequiredBits(sizeof...(MessagePipeTypes) - 1));

    auto visitor = [&](auto & ... elems)
    {
      int channel_index = 0;
      ProcessMessage(channel_index, target_channel, reader, elems...);
    };

    NetMetaUtil::Apply(visitor, m_Receivers);
  }

private:

  struct NetSenderProtocolSource
  {
    int m_ChannelIndex;
    NetTransmitter *m_Transmitter;

    NetBitWriter & CreateAck()
    {
      NetBitWriter & writer = m_Transmitter.CreateWriter(NetPipeMode::kUnreliableSequenced);
      writer.WriteBits(1, 1); // Signal that this is an ack
      writer.WriteBits((uint64_t)m_ChannelIndex, GetRequiredBits(sizeof...(MessagePipeTypes) - 1));
      return writer;
    }

    void SendAck(NetBitWriter & writer)
    {
      m_Transmitter.SendMessage(writer);
    }
  };

  template <typename SenderType, typename ... Args>
  void InitReceiver(int & channel_index, NetTransmitter *transmitter, std::unique_ptr<SenderType> & sender_ptr, Args && ... args)
  {
    sender_ptr = std::make_unique<SenderType>(NetSenderProtocolSource{ channel_index, transmitter });

    channel_index++;
    InitReceiver(channel_index, transmitter, std::forward<Args>(args)...);
  }

  void InitReceiver(int & channel_index, NetTransmitter *transmitter)
  {

  }

  template <typename ReceiverType, typename ... Args>
  void ProcessMessage(int & channel_index, int & target_index, NetBitReader & reader, std::unique_ptr<ReceiverType> & receiver_ptr, Args && ... args)
  {
    if (channel_index == target_index)
    {
      receiver_ptr->GotMessage(reader);
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

  std::tuple<std::unique_ptr<typename MessagePipeTypes::template ReceiverType<NetSenderProtocolSource>>...> m_Receivers;
};
