#pragma once

#include <functional>

#include "NetSerializeDelta.h"
#include "NetDeserializeDelta.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass, class Sink>
class NetPipeFullDeltaSync
{
public:
  NetPipeFullDeltaSync(Sink && sink)
    : m_Sink(sink);
  {

  }

  void RegisterUpdateCallback(std::functional<void(DataClass &)> && func)
  {
    m_UpdateCallback = func;
  }

  void SyncState(DataClass & current_state, const DataClass & reference_state)
  {
    NetBitWriter writer = m_Sink.CreateWriter();
    NetSerializeValueDelta(current_state, reference_state, writer);
    m_Sink(writer);
  }

  void GotMessage(NetBitReader & reader, DataClass & current_state, const DataClass & reference_state)
  {
    StormReflCopy(current_state, reference_state);
    NetDeserializeValueDelta(current_state, reader);

    m_UpdateCallback(m_Inst);
  }

private:
  Sink m_Sink;
};

