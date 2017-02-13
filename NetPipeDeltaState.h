#pragma once

#include <functional>

#include "NetSerializeDelta.h"
#include "NetDeserializeDelta.h"
#include "NetStateStore.h"
#include "NetTransmitter.h"

#ifdef _DEBUG
#include "NetBitWriterVector.h"
#include "NetBitReaderBuffer.h"
#endif

class NetBitWriter;
class NetBitReader;

template <class DataClass, int DataStoreSize>
class NetPipeDeltaStateReceiver;

template <class DataClass, int DataStoreSize>
class NetPipeDeltaStateSender
{
public:

  using ClientType = NetPipeDeltaStateReceiver<DataClass, DataStoreSize>;

  NetPipeDeltaStateSender() :
    m_LastAckedState(-1),
    m_NextStateSlot(-1)
  {

  }

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  void SyncState(const std::shared_ptr<DataClass> & current_state)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);
    DataClass * reference_state = m_LastAckedState != -1 ? m_StateStore.Get(m_LastAckedState) : nullptr;

    int next_state_slot = m_NextStateSlot + 1;
    next_state_slot %= DataStoreSize;

    if (next_state_slot == m_LastAckedState)
    {
      return;
    }

    m_NextStateSlot = next_state_slot;
    m_StateStore.StoreState(current_state, m_NextStateSlot);

    writer.WriteBits(m_NextStateSlot, GetRequiredBits(DataStoreSize - 1));
    writer.WriteBits(m_LastAckedState != -1 ? m_LastAckedState : DataStoreSize, GetRequiredBits(DataStoreSize - 1));

    if (reference_state)
    {
      NetSerializeValueDelta(*current_state.get(), *reference_state, writer);

#ifdef _DEBUG
      NetBitWriterVector test_writer;
      NetSerializeValueDelta(*current_state.get(), *reference_state, test_writer);

      DataClass copy = *reference_state;
      NetBitReaderBuffer reader(test_writer.GetData(), test_writer.GetDataSize());
      NetDeserializeValueDelta(copy, reader);

      if (StormReflCompare(*current_state.get(), copy) == false)
      {
        NetBitWriterVector test_writer;
        NetSerializeValueDelta(*current_state.get(), *reference_state, test_writer);

        DataClass copy = *reference_state;
        NetBitReaderBuffer reader(test_writer.GetData(), test_writer.GetDataSize());
        NetDeserializeValueDelta(copy, reader);

        return;
      }
#endif
    }
    else
    {
      NetSerializeValue(*current_state.get(), writer);
    }

    m_Transmitter->SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {
    m_LastAckedState = (int)reader.ReadUBits(GetRequiredBits(DataStoreSize - 1));
  }

private:

  NetTransmitter * m_Transmitter;
  NetPipeMode m_Mode;
  int m_ChannelIndex;
  int m_ChannelBits;

  int m_LastAckedState;
  int m_NextStateSlot;
  NetStateStore<DataClass, DataStoreSize> m_StateStore;
};


template <class DataClass, int DataStoreSize>
class NetPipeDeltaStateReceiver
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
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
    int state_slot = (int)reader.ReadUBits(GetRequiredBits(DataStoreSize - 1));
    int ref_slot = (int)reader.ReadUBits(GetRequiredBits(DataStoreSize));

    std::shared_ptr<DataClass> inst = std::make_shared<DataClass>();

    if (ref_slot != DataStoreSize)
    {
      DataClass * reference_state = m_StateStore.Get(ref_slot);

      StormReflCopy(*inst.get(), *reference_state);
      NetDeserializeValueDelta(*inst.get(), reader);
    }
    else
    {
      NetDeserializeValue(*inst.get(), reader);
    }

    m_StateStore.StoreState(inst, state_slot);

    if (m_UpdateCallback)
    {
      m_UpdateCallback(*inst.get());
    }

    NetBitWriter & writer = m_Transmitter->CreateAck(m_Mode, m_ChannelIndex, m_ChannelBits);
    writer.WriteBits(state_slot, GetRequiredBits(DataStoreSize - 1));

    m_Transmitter->SendAck(writer);
  }

private:

  NetTransmitter * m_Transmitter;
  NetPipeMode m_Mode;
  int m_ChannelIndex;
  int m_ChannelBits;

  std::function<void(const DataClass &)> m_UpdateCallback;
  NetStateStore<DataClass, DataStoreSize> m_StateStore;
};


template <class DataClass, int DataStoreSize, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeDeltaState
{
  using SenderType = NetPipeDeltaStateSender<DataClass, DataStoreSize>;

  using ReceiverType = NetPipeDeltaStateReceiver<DataClass, DataStoreSize>;

  static const NetPipeMode PipeMode = Mode;
};
