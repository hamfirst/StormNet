#pragma once

#include <unordered_map>

#include "NetBitWriter.h"
#include "NetSerialize.h"
#include "NetTransmitter.h"

template <class BaseClass>
class NetPipeMessageSender;

template <typename BaseClass>
class NetMessageSender
{
public:
  NetMessageSender(NetPipeMessageSender<BaseClass> & pipe) :
    m_Transmitter(pipe.m_Transmitter),
    m_Mode(pipe.m_Mode),
    m_ChannelIndex(pipe.m_ChannelIndex),
    m_ChannelBits(pipe.m_ChannelBits)
  {

  }

  NetMessageSender(const NetMessageSender & rhs) = default;
  NetMessageSender(NetMessageSender && rhs) = default;

  NetMessageSender & operator = (const NetMessageSender & rhs) = default;
  NetMessageSender & operator = (NetMessageSender && rhs) = default;


  template <class DataType>
  void SendMessage(const DataType & data)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.template GetClassId<DataType>();

    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);

    writer.WriteBits(class_id, GetRequiredBits(type_db.GetNumTypes() - 1));
    NetSerializeValue(data, writer);

    m_Transmitter->SendMessage(writer);
  }

  void SendMessage(std::size_t class_id, const void * event_ptr)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto & type_info = type_db.GetTypeInfo(class_id);

    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);
    writer.WriteBits(class_id, GetRequiredBits(type_db.GetNumTypes() - 1));
    type_info.m_Serialize(event_ptr, writer);
    m_Transmitter->SendMessage(writer);
  }

private:

  NetTransmitter * m_Transmitter;
  NetPipeMode m_Mode;
  int m_ChannelIndex;
  int m_ChannelBits;
};


template <typename BaseClass>
class NetMessageSenderList
{
public:

  template <class DataType>
  void SendMessage(const DataType & data)
  {
    for (auto & sender : m_Senders)
    {
      sender.second.SendMessage(data);
    }
  }

  template <class DataType, typename Predicate>
  void SendMessage(const DataType & data, Predicate && predicate)
  {
    for (auto & sender : m_Senders)
    {
      if (predicate(sender.first))
      {
        sender.second.SendMessage(data);
      }
    }
  }

  template <class DataType>
  void SendTargettedMessage(const DataType & data, std::size_t connection)
  {
    auto itr = m_Senders.find(connection);
    if (itr == m_Senders.end())
    {
      return;
    }

    itr->second.SendMessage(data);
  }

  void SendMessage(std::size_t class_id, const void * event_ptr)
  {
    for (auto & sender : m_Senders)
    {
      if (predicate(sender.first))
      {
        sender.second.SendMessage(class_id, event_ptr);
      }
    }
  }

  void SendMessage(std::size_t class_id, const void * event_ptr, std::size_t connection)
  {
    auto itr = m_Senders.find(connection);
    if (itr == m_Senders.end())
    {
      return;
    }

    itr->second.SendMessage(class_id, event_ptr);
  }

  void AddSender(std::size_t connection_id, const NetMessageSender<BaseClass> & sender)
  {
    m_Senders.emplace(std::make_pair(connection_id, sender));
  }

  void RemoveSender(std::size_t connection_id)
  {
    m_Senders.erase(connection_id);
  }

private:
  std::unordered_map<std::size_t, NetMessageSender<BaseClass>> m_Senders;
};

