#pragma once

#include <functional>

#include "NetSerializeDelta.h"
#include "NetDeserializeDelta.h"
#include "NetStateStore.h"
#include "NetPipeMode.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass, class Sink, int DataStoreSize>
class NetPipeDeltaStateSender
{
public:

  template <class Source>
  using ClientType = NetPipeDeltaStateReceiver<DataClass, Source, DataStoreSize>;

  NetPipeDeltaStateSender(Sink && sink) :
    m_Sink(sink),
    m_LastAckedState(-1),
    m_NextStateSlot(-1)
  {

  }

  void SyncState(const std::shared_ptr<DataClass> & current_state)
  {
    NetBitWriter & writer = m_Sink.CreateMessage();
    DataClass * reference_state = m_LastAckedState != -1 ? m_StateStore.Get(m_LastAckedState) : nullptr;

    m_NextStateSlot++;
    m_NextStateSlot %= DataStoreSize;

    m_StateStore.StoreState(current_state, m_NextStateSlot);

    writer->WriteBits(m_NextStateSlot, GetRequiredBits(DataStoreSize - 1));

    if (reference_state)
    {
      NetSerializeValueDelta(*current_state.get(), *reference_state, writer);
    }
    else
    {
      NetSerializeValue(*current_state.get(), writer);
    }

    m_Sink.SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {
    m_LastAckedState = (int)reader.ReadUBits(GetRequiredBits(DataStoreSize - 1));
  }

private:
  Sink m_Sink;

  int m_LastAckedState;
  int m_NextStateSlot;
  NetStateStore<DataClass, DataStoreSize> m_StateStore;
};


template <class DataClass, class Source, int DataStoreSize>
class NetPipeDeltaStateReceiver
{
public:
  NetPipeDeltaStateReceiver(Source && source) :
    m_Source(source)
  {

  }

  void RegisterUpdateCallback(std::function<void(const DataClass &)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  void GotMessage(NetBitReader & reader)
  {
    int state_slot = (int)reader.ReadUBits(GetRequiredBits(DataStoreSize - 1));

    DataClass * reference_state = m_StateStore.Get();
    std::shared_ptr<DataClass> inst = std::make_shared<DataClass>();

    if (inst)
    {
      StormReflCopy(inst.get(), *reference_state);
      NetDeserializeValueDelta(reference_state, reader);
    }
    else
    {
      NetDeserializeValue(inst.get(), reader);
    }

    m_StateStore.StoreState(inst, state_slot);
    m_UpdateCallback(*inst.get());

    NetBitWriter writer = m_Source.CreateAck();
    writer.WriteBits(state_slot, GetRequiredBits(DataStoreSize - 1));

    m_Source.SendAck(writer);
  }

private:
  Source m_Source;

  std::function<void(const DataClass &)> m_UpdateCallback;
  NetStateStore<DataClass, DataStoreSize> m_StateStore;
};


template <class DataClass, int DataStoreSize, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeDeltaState
{
  template <typename Sink>
  using SenderType = NetPipeDeltaStateSender<DataClass, Sink, DataStoreSize>;

  template <typename Source>
  using ReceiverType = NetPipeDeltaStateReceiver<DataClass, Source, DataStoreSize>;

  static const NetPipeMode PipeMode = Mode;

  template <typename Sink>
  auto MakeSender(Sink && sink)
  {
    return NetPipeDeltaStateSender<DataClass, Sink, DataStoreSize>(std::forward<Sink>(sink));
  }

  template <typename Source>
  auto MakeReceiver(Source && source)
  {
    return NetPipeDeltaStateReceiver<DataClass, Source, DataStoreSize>(std::forward<Source>(source));
  }
};
