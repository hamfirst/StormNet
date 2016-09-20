#pragma once

#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass, class Sink, class UpdateCallback>
class NetPipeFullStateSync
{
public:
  NetPipeFullStateSync(Sink && sink, UpdateCallback && callback)
    : m_Inst(), m_Sink(sink), m_UpdateCallback(callback);
  {

  }

  void RegisterUpdateCallback(std::functional<void(DataClass &)> && func)
  {
    m_UpdateCallback = func;
  }

  void SyncState()
  {
    NetBitWriter writer = m_Sink.CreateWriter();
    NetSerializeValue(m_Inst, writer);
    m_Sink(writer);
  }

  void GotMessage(NetBitReader & reader)
  {
    NetDeserializeValue(m_Inst, reader);
    m_UpdateCallback(m_Inst);
  }

private:

  DataClass m_Inst;
  Sink m_Sink;
  UpdateCallback m_UpdateCallback;
};

