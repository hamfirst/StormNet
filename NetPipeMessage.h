#pragma once

#include <memory>
#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetPipeMode.h"

class NetBitWriter;
class NetBitReader;

template <class BaseClass, class Sink>
class NetPipeMessageSender
{
public:

  NetPipeMessageSender(const Sink & sink) :
    m_Sink(sink)
  {

  }

  template <class DataType>
  void SendMessage(const DataType & data)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.GetClassId<DataType>();

    NetBitWriter & writer = m_Sink.CreateMessage();

    writer.WriteBits(class_id, GetRequiredBits(type_db.GetNumTypes() - 1));
    NetSerializeValue(data, writer);

    m_Sink.SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {

  }

private:

  Sink m_Sink;
};

template <class BaseClass, class Source>
class NetPipeMessageReceiver
{
public:

  NetPipeMessageReceiver(const Source & source) :
    m_Source(source)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    m_Callbacks = std::make_unique<std::function<void(NetBitReader &)>[]>(type_db.GetNumTypes());
  }


  template <typename DataType>
  void RegisterCallback(void(*func)(const DataType &))
  {
    RegisterCallbackInteral<DataType, decltype(func)>(func);
  }

  template <typename C, typename DataType>
  void RegisterCallback(void(C::*func)(const DataType &), C * c)
  {
    RegisterCallbackInteral<DataType>([=](const DataType & data) { c->*func(data); });
  }

  template <typename DataType, typename Callback>
  void RegisterCallback(Callback callback)
  {
    RegisterCallbackInteral<DataType>(callback);
  }

  void GotMessage(NetBitReader & reader)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = reader.ReadUBits(GetRequiredBits(BaseClass::__s_TypeDatabase.GetNumTypes() - 1));

    if (m_Callbacks[class_id])
    {
      m_Callbacks[class_id](reader);
    }
  }

protected:

  template <class First, class... Args>
  void RegisterCallbackPack(First && first, Args &&... args)
  {
    RegisterCallbackDeduce(first);
    RegisterCallbackPack(args...);
  }

  void RegisterCallbackPack()
  {

  }

  template <class DataType, class CallbackType>
  void RegisterCallbackInteral(CallbackType callback)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.GetClassId<DataType>();

    auto deserialize_cb = [=](NetBitReader & reader)
    {
      DataType dt;
      NetDeserializeValue(dt, reader);
      callback(dt);
    };

    m_Callbacks[class_id] = deserialize_cb;
  }

private:
  std::unique_ptr<std::function<void(NetBitReader &)>[]> m_Callbacks;
  Source m_Source;
};

template <class BaseClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeMessage
{
  template <typename Sink>
  using SenderType = NetPipeMessageSender<BaseClass, Sink>;

  template <typename Source>
  using ReceiverType = NetPipeMessageReceiver<BaseClass, Source>;

  static const NetPipeMode PipeMode = Mode;

  template <typename Sink>
  auto MakeSender(Sink && sink)
  {
    return NetPipeMessageSender<BaseClass, Sink>(std::forward<Sink>(sink));
  }

  template <typename Source>
  auto MakeReceiver(Source && source)
  {
    return NetPipeMessageSender<BaseClass, Source>(std::forward<Source>(source));
  }
};
