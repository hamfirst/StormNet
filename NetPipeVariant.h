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

template <class T, class ... Types>
struct NetPipeVariantUtil
{
  static constexpr int GetTypeIndex(int i = 0)
  {
    return -1;
  }
};

template <class T, class Test, class ... Types>
struct NetPipeVariantUtil<T, Test, Types...>
{
  static constexpr int GetTypeIndex(int i = 0)
  {
    return std::is_same<T, Test>::value ? i : NetPipeVariantUtil<T, Types...>::GetTypeIndex(i + 1);
  }
};

template <class ... Types>
class NetPipeVariantSender
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
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);

    auto class_id = NetPipeVariantUtil<DataType, Types...>::GetTypeIndex();
    if (class_id == -1)
    {
      throw false;
    }

    writer.WriteBits(class_id, GetRequiredBits(sizeof...(Types) - 1));
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

template <class ... Types>
class NetPipeVariantReceiver
{
public:

  NetPipeVariantReceiver()
  {
    m_Callbacks = std::make_unique<std::function<void(NetBitReader &)>[]>(sizeof...(Types));
  }

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_DefaultData = std::make_tuple(StormReflGetDefault<Types>() ...);
  }

  template <typename DataType>
  void SetDefault(DataType * data_type)
  {
    auto & default_data = std::get<DataType *>(m_DefaultData);
    default_data = data_type;
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

  template <typename DataType>
  void RegisterCallback(void(*func)(DataType &&))
  {
    RegisterCallbackInteral<DataType, decltype(func)>(func);
  }

  template <typename C, typename DataType>
  void RegisterCallback(void(C::*func)(DataType &&), C * c)
  {
    auto callback_func = [=](DataType && data) { (c->*func)(std::move(data)); };
    RegisterCallbackInteral<DataType, decltype(callback_func)>(callback_func);
  }

  template <typename DataType, typename Callback>
  void RegisterCallback(Callback callback)
  {
    RegisterCallbackInteral<DataType, Callback>(callback);
  }

  void GotMessage(NetBitReader & reader)
  {
    auto class_id = (std::size_t)reader.ReadUBits(GetRequiredBits(sizeof...(Types) - 1));
    if (m_Callbacks[class_id])
    {
      m_Callbacks[class_id](reader);
    }
  }

protected:

  template <class DataType, class CallbackType>
  void RegisterCallbackInteral(CallbackType & callback)
  {
    auto class_id = NetPipeVariantUtil<DataType, Types...>::GetTypeIndex();
    if (class_id == -1)
    {
      throw false;
    }

    auto deserialize_cb = [=](NetBitReader & reader)
    {
      auto default_data = std::get<DataType *>(m_DefaultData);

      DataType dt = *default_data;
      NetDeserializeValue(dt, reader);
      callback(std::move(dt));
    };

    m_Callbacks[class_id] = std::move(deserialize_cb);
  }

private:
  std::unique_ptr<std::function<void(NetBitReader &)>[]> m_Callbacks;
  std::tuple<Types * ...> m_DefaultData;
};

template <NetPipeMode Mode, class ... Types>
struct NetPipeVariant
{
  using SenderType = NetPipeVariantSender<Types...>;
  using ReceiverType = NetPipeVariantReceiver<Types...>;

  static const NetPipeMode PipeMode = Mode;
};
