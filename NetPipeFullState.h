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

  }

  template <typename C>
  void RegisterCallback(void(C::*func)(DataClass &&), C * c)
  {
    auto callback_func = [=](DataClass && inst) { (c->*func)(std::move(inst)); };
    RegisterCallback(callback_func);
  }

  void RegisterCallback(std::function<void(DataClass &&)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  void GotMessage(NetBitReader & reader)
  {
    DataClass inst = {};
    NetDeserializeValue(inst, reader);

    if (m_UpdateCallback)
    {
      m_UpdateCallback(std::move(inst));
    }
  }

private:

  std::function<void(DataClass &&)> m_UpdateCallback;
};


template <class DataClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeFullState
{
  using SenderType = NetPipeFullStateSender<DataClass>;
  using ReceiverType = NetPipeFullStateReciever<DataClass>;

  static const NetPipeMode PipeMode = Mode;
};
