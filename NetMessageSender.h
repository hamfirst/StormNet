#pragma once

#include <unordered_map>

#include "NetBitWriter.h"
#include "NetSerialize.h"
#include "NetPipeSink.h"

template <class BaseClass, class Sink>
class NetPipeMessageSender;

template <typename BaseClass>
class NetMessageSender
{
public:
  template <class Sink>
  NetMessageSender(NetPipeMessageSender<BaseClass, Sink> & pipe) :
    m_Sink(&pipe.m_Sink)
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
    auto class_id = type_db.GetClassId<DataType>();

    NetBitWriter & writer = m_Sink->CreateMessage();

    writer.WriteBits(class_id, GetRequiredBits(type_db.GetNumTypes() - 1));
    NetSerializeValue(data, writer);

    m_Sink->SendMessage(writer);
  }

private:

  NetPipeSink * m_Sink;
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

