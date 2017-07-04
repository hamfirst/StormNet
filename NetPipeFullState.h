#pragma once

#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetTransmitter.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass>
class NetPipeFullStateSender
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  void SyncState(const DataClass & state)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);
    NetSerializeValue(state, writer);
    m_Transmitter->SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {

  }

private:
  NetTransmitter * m_Transmitter;
  NetPipeMode m_Mode;
  int m_ChannelIndex;
  int m_ChannelBits;
};

template <class DataClass>
class NetPipeFullStateReciever
{
public:
  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_DefaultData = StormReflGetDefault<DataClass>();
  }

  template <typename C>
  void RegisterCallback(void(C::*func)(DataClass &&), C * c)
  {
    auto callback_func = [=](DataClass && inst) { (c->*func)(std::move(inst)); };
    RegisterCallback(callback_func);
  }

  void SetDefault(DataClass * default_data_ptr)
  {
    m_DefaultData = default_data_ptr;
  }

  void RegisterCallback(std::function<void(DataClass &&)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  template <typename C>
  void RegisterCallback(void(C::*func)(const DataClass &), C * c)
  {
    auto callback_func = [=](const DataClass & inst) { (c->*func)(inst); };
    RegisterCallback(callback_func);
  }

  void GotMessage(NetBitReader & reader)
  {
    if (m_DefaultData == nullptr)
    {
      return;
    }

    DataClass inst = *m_DefaultData;
    NetDeserializeValue(inst, reader);

    if (m_UpdateCallback)
    {
      m_UpdateCallback(std::move(inst));
    }
  }

private:

  std::function<void(DataClass &&)> m_UpdateCallback;
  DataClass * m_DefaultData;
};

template <class DataClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeFullState
{
  using SenderType = NetPipeFullStateSender<DataClass>;
  using ReceiverType = NetPipeFullStateReciever<DataClass>;

  static const NetPipeMode PipeMode = Mode;
};
