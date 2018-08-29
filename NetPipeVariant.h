#pragma once

#include <memory>
#include <functional>
#include <type_traits>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetPipeMode.h"
#include "NetMetaUtil.h"

class NetBitWriter;
class NetBitReader;
class NetTransmitter;

template <typename ... Types>
struct NetPipeVariantInit
{
  static void Process(void(**func_list)(const void *, NetBitWriter &))
  {

  }
};

template <typename Type, typename ... Types>
struct NetPipeVariantInit<Type, Types...>
{
  static void Process(void(**func_list)(const void * data, NetBitWriter & writer))
  {
    *func_list = [](const void * data, NetBitWriter & writer) 
    {
      NetSerializeValue(*static_cast<const Type *>(data), writer);
    };

    NetPipeVariantInit<Types...>::Process(func_list + 1);
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

    m_Serializers = std::make_unique<SerializerFunc[]>(sizeof...(Types));
    NetPipeVariantInit<Types...>::Process(m_Serializers.get());
  }

  template <class DataType>
  void SendMessage(const DataType & data)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);

    auto class_id = NetMetaUtil::GetTypeIndex<DataType, Types...>();
    if (class_id == -1)
    {
      throw false;
    }

    writer.WriteBits(class_id, GetRequiredBits(sizeof...(Types) - 1));
    NetSerializeValue(data, writer);

    m_Transmitter->SendMessage(writer);
  }

  void SendMessage(int type_index, const void * data)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);

    writer.WriteBits(type_index, GetRequiredBits(sizeof...(Types)-1));
    m_Serializers[type_index](data, writer);

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

  using SerializerFunc = void(*)(const void *, NetBitWriter &);
  std::unique_ptr<SerializerFunc[]> m_Serializers;
};

template <int Index, typename ... Types>
struct NetPipeVariantReceiverInit
{
  template <typename DefaultData>
  static void Process(std::function<void(NetBitReader &, void *, std::function<void(std::size_t, void *)> &)> * funcs)
  {

  }
};

template <int Index, typename Type, typename ... Types>
struct NetPipeVariantReceiverInit<Index, Type, Types...>
{
  template <typename DefaultData>
  static void Process(std::function<void(NetBitReader &, void *, std::function<void(std::size_t, void *)> &)> * funcs)
  {
    *funcs = [&](NetBitReader & reader, void * def, std::function<void(std::size_t, void *)> & default_callback)
    {
      DefaultData * default_data = (DefaultData *)def;
      Type * data = std::template get<Index>(*default_data);
      if (data)
      {
        Type new_elem(*data);
        NetDeserializeValue(new_elem, reader);
        default_callback(Index, &new_elem);
      }
      else
      {
        Type new_elem;
        NetDeserializeValue(new_elem, reader);
        default_callback(Index, &new_elem);
      }
    };


    NetPipeVariantReceiverInit<Index + 1, Types...>::template Process<DefaultData>(funcs + 1);
  }
};

template <class ... Types>
class NetPipeVariantReceiver
{
public:

  NetPipeVariantReceiver()
  {
    m_Callbacks = std::make_unique<std::function<void(NetBitReader &)>[]>(sizeof...(Types));
    m_CallGenericCallbacks = std::make_unique<std::function<void(NetBitReader &, void *, std::function<void(std::size_t, void *)> &)>[]>(sizeof...(Types));
  }

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_DefaultData = std::make_tuple(StormReflGetDefault<Types>() ...);
    NetPipeVariantReceiverInit<0, Types...>::template Process<std::tuple<Types * ...>>(m_CallGenericCallbacks.get());
  }

  template <typename DataType>
  void SetDefault(DataType * data_type)
  {
    auto & default_data = std::template get<DataType *>(m_DefaultData);
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

  template <typename Callback>
  void RegisterGenericCallback(Callback && callback)
  {
    m_GenericCallback = callback;
  }

  template <typename C>
  void RegisterGenericCallback(void(C::*func)(std::size_t, void *), C * c)
  {
    auto callback_func = [=](std::size_t class_id, void * message_ptr) { (c->*func)(class_id, message_ptr); };
    m_GenericCallback = callback_func;
  }

  void GotMessage(NetBitReader & reader)
  {
    auto class_id = (std::size_t)reader.ReadUBits(GetRequiredBits(sizeof...(Types) - 1));
    if (m_Callbacks[class_id])
    {
      m_Callbacks[class_id](reader);
    }
    else if (m_GenericCallback)
    {
      m_CallGenericCallbacks[class_id](reader, &m_DefaultData, m_GenericCallback);
    }
  }

protected:

  template <class DataType, class CallbackType>
  void RegisterCallbackInteral(CallbackType & callback)
  {
    auto class_id = NetMetaUtil::template GetTypeIndex<DataType, Types...>();
    if (class_id == -1)
    {
      throw false;
    }

    auto deserialize_cb = [=](NetBitReader & reader)
    {
      auto default_data = std::template get<DataType *>(m_DefaultData);

      DataType dt = *default_data;
      NetDeserializeValue(dt, reader);
      callback(std::move(dt));
    };

    m_Callbacks[class_id] = std::move(deserialize_cb);
  }

private:
  std::function<void(std::size_t, void *)> m_GenericCallback;
  std::unique_ptr<std::function<void(NetBitReader &)>[]> m_Callbacks;
  std::unique_ptr<std::function<void(NetBitReader &, void *, std::function<void(std::size_t, void *)> &)>[]> m_CallGenericCallbacks;
  std::tuple<Types * ...> m_DefaultData;

  using SerializerFunc = void(*)(NetBitReader &);
  std::unique_ptr<SerializerFunc[]> m_Serializers;
};

template <NetPipeMode Mode, class ... Types>
struct NetPipeVariant
{
  using SenderType = NetPipeVariantSender<Types...>;
  using ReceiverType = NetPipeVariantReceiver<Types...>;

  static const NetPipeMode PipeMode = Mode;
};
