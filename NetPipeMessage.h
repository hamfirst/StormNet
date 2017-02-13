#pragma once

#include <memory>
#include <functional>
#include <type_traits>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetPipeMode.h"

class NetBitWriter;
class NetBitReader;
class NetTransmitter;

template <class BaseClass>
class NetPipeMessageSender
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  template <class DataType>
  void SendMessage(const DataType & data)
  {
    static_assert(std::is_base_of<BaseClass, DataType>::value, "Must send a data type that derives from the base type");

    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.template GetClassId<DataType>();

    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);

    writer.WriteBits(class_id, GetRequiredBits(type_db.GetNumTypes() - 1));
    NetSerializeValue(data, writer);

    m_Transmitter->SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {

  }

private:

  template <typename SenderBaseClass>
  friend class NetMessageSender;

  NetTransmitter * m_Transmitter;
  NetPipeMode m_Mode;
  int m_ChannelIndex;
  int m_ChannelBits;
};

template <class BaseClass>
class NetPipeMessageReceiver
{
public:

  NetPipeMessageReceiver()
  {
    auto & type_db = BaseClass::__s_TypeDatabase;

    auto num_types = type_db.GetNumTypes();
    m_Callbacks = std::make_unique<std::function<void(NetBitReader &)>[]>(num_types);
  }

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {

  }

  template <typename DataType>
  void RegisterCallback(void(*func)(const DataType &))
  {
    RegisterCallbackInteral<DataType, decltype(func)>(func);
  }

  template <typename C, typename DataType>
  void RegisterCallback(void(C::*func)(const DataType &), C * c)
  {
    auto callback_func = [=](const DataType & data) { (c->*func)(data); };
    RegisterCallbackInteral<DataType, decltype(callback_func)>(callback_func);
  }

  template <typename DataType, typename Callback>
  void RegisterCallback(Callback callback)
  {
    RegisterCallbackInteral<DataType, Callback>(callback);
  }

  void GotMessage(NetBitReader & reader)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = (std::size_t)reader.ReadUBits(GetRequiredBits(BaseClass::__s_TypeDatabase.GetNumTypes() - 1));

    if (class_id >= type_db.GetNumTypes())
    {
      return;
    }

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
  void RegisterCallbackInteral(CallbackType & callback)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.template GetClassId<DataType>();

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
};

template <class BaseClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeMessage
{
  using SenderType = NetPipeMessageSender<BaseClass>;

  using ReceiverType = NetPipeMessageReceiver<BaseClass>;

  static const NetPipeMode PipeMode = Mode;
};
