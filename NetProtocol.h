#pragma once

#include "NetProtocolSender.h"
#include "NetProtocolReceiver.h"


template <typename ... MessagePipeTypes>
class NetProtocolDefinition
{

};

class NetProtocol
{
public:
  virtual void GotMessage(NetBitReader & reader) = 0;
};

template <typename ... MessagePipeTypes>
class NetProtocolBuilder
{
public:

  class SymmetricProtocol : public NetProtocol
  {
  public:
    SymmetricProtocol(NetTransmitter * transmitter) :
      m_Senders(transmitter, GetRequiredBits(sizeof...(MessagePipeTypes)-1)),
      m_Receivers(transmitter, GetRequiredBits(sizeof...(MessagePipeTypes)-1))
    {

    }

    template <int ChannelIndex>
    auto & GetSenderChannel()
    {
      return m_Senders.template GetChannel<ChannelIndex>();
    }

    template <int ChannelIndex>
    auto & GetReceiverChannel()
    {
      return m_Receivers.template GetChannel<ChannelIndex>();
    }

    void GotMessage(NetBitReader & reader) override
    {
      int is_ack = (int)reader.ReadUBits(1);
      if (is_ack)
      {
        m_Senders.GotAck(reader);
      }
      else
      {
        m_Receivers.GotMessage(reader);
      }
    }

  private:
    NetProtocolSender<MessagePipeTypes...> m_Senders;
    NetProtocolReceiver<MessagePipeTypes...> m_Receivers;
  };

  template <typename ... OppositeMessagePipeTypes>
  class AsymmetricProtocol : public NetProtocol
  {
  public:
    AsymmetricProtocol(NetTransmitter *transmitter) :
      m_Senders(transmitter, GetRequiredBits(sizeof...(MessagePipeTypes)-1)),
      m_Receivers(transmitter, GetRequiredBits(sizeof...(MessagePipeTypes)-1))
    {

    }

    template <int ChannelIndex>
    auto & GetSenderChannel()
    {
      return m_Senders.template GetChannel<ChannelIndex>();
    }

    template <int ChannelIndex>
    auto & GetReceiverChannel()
    {
      return m_Receivers.template GetChannel<ChannelIndex>();
    }

    void GotMessage(NetBitReader & reader) override
    {
      int is_ack = (int)reader.ReadUBits(1);
      if (is_ack)
      {
        m_Senders.GotAck(reader);
      }
      else
      {
        m_Receivers.GotMessage(reader);
      }
    }

  private:
    NetProtocolSender<OppositeMessagePipeTypes...> m_Senders;
    NetProtocolReceiver<MessagePipeTypes...> m_Receivers;
  };
};

template <typename ... MessagePipeTypes>
auto NetCreateSymmetricProtocol(NetTransmitter * transmitter, const NetProtocolDefinition<MessagePipeTypes...> & protocol_def)
{
  return typename NetProtocolBuilder<MessagePipeTypes...>::SymmetricProtocol(transmitter);
}

template <typename ... LocalMessagePipeTypes, typename ... RemoteMessagePipeTypes>
auto NetCreateAsymmetricProtocol(NetTransmitter * transmitter, 
  const NetProtocolDefinition<LocalMessagePipeTypes...> & local_protocol_def, const NetProtocolDefinition<RemoteMessagePipeTypes...> & remote_protocol_def)
{
  return typename NetProtocolBuilder<LocalMessagePipeTypes...>::template AsymmetricProtocol<RemoteMessagePipeTypes...>(transmitter);
}

template <typename LocalProtoDef>
struct NetProtocolInfo
{
  using SymmetricProtocolType = decltype(NetCreateSymmetricProtocol(nullptr, LocalProtoDef{}));

  template <typename RemoteProtoDef>
  using AsymmetricProtocolType = decltype(NetCreateAsymmetricProtocol(nullptr, LocalProtoDef{}, RemoteProtoDef{}));
};

