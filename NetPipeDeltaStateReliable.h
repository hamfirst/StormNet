#pragma once

#include <functional>
#include <optional/optional.hpp>

#include "NetSerializeDelta.h"
#include "NetDeserializeDelta.h"
#include "NetStateStore.h"
#include "NetTransmitter.h"

class NetBitWriter;
class NetBitReader;


template <class DataClass>
class NetPipeDeltaStateReliableSender
{
public:

  NetPipeDeltaStateReliableSender()
  {

  }

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  void SyncState(const std::shared_ptr<DataClass> & current_state)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(NetPipeMode::kReliable, m_ChannelIndex, m_ChannelBits);

    if (m_LastSyncedState)
    {
      NetSerializeValueDelta(*current_state.get(), *m_LastSyncedState.get(), writer);
    }
    else
    {
      NetSerializeValue(*current_state.get(), writer);
    }

    m_LastSyncedState = current_state;

    m_Transmitter->SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {

  }

private:

  NetTransmitter * m_Transmitter;
  int m_ChannelIndex;
  int m_ChannelBits;

  std::shared_ptr<DataClass> m_LastSyncedState;
};


template <class DataClass>
class NetPipeDeltaStateReliableReceiver
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {

  }

  template <typename C>
  void RegisterCallback(void(C::*func)(const DataClass &), C * c)
  {
    auto callback_func = [=](const DataClass & inst) { (c->*func)(inst); };
    RegisterCallback(callback_func);
  }

  void RegisterCallback(std::function<void(const DataClass &)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  void GotMessage(NetBitReader & reader)
  {
    std::shared_ptr<DataClass> inst = std::make_shared<DataClass>();

    if (m_ReferenceState)
    {
      StormReflCopy(*inst.get(), *m_ReferenceState.get());
      NetDeserializeValueDelta(*inst.get(), reader);
    }
    else
    {
      NetDeserializeValue(*inst.get(), reader);
    }

    m_ReferenceState = inst;

    if (m_UpdateCallback)
    {
      m_UpdateCallback(*inst.get());
    }
  }

private:

  std::function<void(const DataClass &)> m_UpdateCallback;
  std::shared_ptr<DataClass> m_ReferenceState;
};

template <class DataClass>
struct NetPipeDeltaStateReliable
{
  using SenderType = NetPipeDeltaStateReliableSender<DataClass>;

  using ReceiverType = NetPipeDeltaStateReliableReceiver<DataClass>;

  static const NetPipeMode PipeMode = NetPipeMode::kReliable;
};
