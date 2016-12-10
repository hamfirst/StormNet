#pragma once

#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetPipeMode.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass, class Sink>
class NetPipeFullStateSender
{
public:

  NetPipeFullStateSender(Sink && sink) : 
    m_Sink(sink)
  {

  }

  void SyncState(DataClass & state)
  {
    NetBitWriter & writer = m_Sink.CreateMessage();
    NetSerializeValue(state, writer);
    m_Sink.SendMessage(writer);
  }

  void GotAck(NetBitReader & reader)
  {

  }

private:

  Sink m_Sink;
};

template <class DataClass, class Source>
class NetPipeFullStateReciever
{
public:
  NetPipeFullStateReciever(Source && source) :
    m_Source(source)
  {

  }

  void RegisterUpdateCallback(std::function<void(DataClass &&)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  void GotMessage(NetBitReader & reader)
  {
    DataClass inst = {};
    NetDeserializeValue(inst, reader);
    m_UpdateCallback(std::move(inst));
  }

private:

  Source m_Source;
  std::function<void(DataClass &&)> m_UpdateCallback;
};


template <class DataClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeFullState
{
  template <typename Sink>
  using SenderType = NetPipeFullStateSender<DataClass, Sink>;

  template <typename Source>
  using ReceiverType = NetPipeFullStateReciever<DataClass, Source>;

  static const NetPipeMode PipeMode = Mode;

  template <typename Sink>
  auto MakeSender(Sink && sink)
  {
    return NetPipeFullStateSender<DataClass, Sink>(std::forward<Sink>(sink));
  }

  template <typename Source>
  auto MakeReceiver(Source && source)
  {
    return NetPipeFullStateReciever<DataClass, Source>(std::forward<Source>(source));
  }
};
