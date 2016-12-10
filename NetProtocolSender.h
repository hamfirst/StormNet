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
  NetProtocolSender(NetTransmitter *transmitter)
  {
    auto initializer = [&](auto & ... elems)
    {
      int channel_index = 0;
      InitSender(channel_index, transmitter, elems...);
    };

    NetMetaUtil::Apply(initializer, m_Senders);
  }

  template<std::size_t ChannelIndex>
  auto & GetChannel()
  {
    return *std::get<ChannelIndex>(m_Senders).second.get();
  }

  void GotAck(NetBitReader & reader)
  {
    int target_channel = (int)reader.ReadUBits(GetRequiredBits(sizeof...(MessagePipeTypes)) - 1);

    auto visitor = [&](auto & ... elems)
    {
      int channel_index = 0;
      ProcessAck(channel_index, target_channel, reader, elems...);
    };

    NetMetaUtil::Apply(visitor, m_Senders);
  }

private:

  struct NetSenderProtocolSink
  {
    int m_ChannelIndex;
    NetTransmitter *m_Transmitter;
    NetPipeMode m_Mode;

    NetBitWriter & CreateMessage()
    {
      NetBitWriter & writer = m_Transmitter->CreateWriter(m_Mode);
      writer.WriteBits(0, 1); // Signal that this is not an ack
      writer.WriteBits((uint64_t)m_ChannelIndex, GetRequiredBits(sizeof...(MessagePipeTypes) - 1));
      return writer;
    }

    void SendMessage(NetBitWriter & writer)
    {
      m_Transmitter->SendMessage(writer);
    }
  };

  template <typename MessagePipeType, typename SenderType, typename ... Args>
  void InitSender(int & channel_index, NetTransmitter *transmitter, std::pair<MessagePipeType, std::unique_ptr<SenderType>> & sender_ptr, Args && ... args)
  {
    sender_ptr.second = std::make_unique<SenderType>(NetSenderProtocolSink{ channel_index, transmitter, MessagePipeType::PipeMode });

    channel_index++;
    InitSender(channel_index, transmitter, std::forward<Args>(args)...);
  }

  void InitSender(int & channel_index, NetTransmitter *transmitter)
  {

  }

  template <typename MessagePipeType, typename SenderType, typename ... Args>
  void ProcessAck(int & channel_index, int & target_index, NetBitReader & reader, std::pair<MessagePipeType, std::unique_ptr<SenderType>> & sender_ptr, Args && ... args)
  {
    if (channel_index == target_index)
    {
      sender_ptr.second->GotAck(reader);
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

  std::tuple<std::pair<MessagePipeTypes, std::unique_ptr<typename MessagePipeTypes::template SenderType<NetSenderProtocolSink>>>...> m_Senders;
};
