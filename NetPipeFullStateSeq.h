<<<<<<< HEAD
#pragma once

#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetTransmitter.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass>
class NetPipeFullStateSeqSender
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  void SyncState(uint32_t seq, const DataClass & state)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);
    writer.WriteBits(seq, 32);
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
class NetPipeFullStateSeqReciever
{
public:
  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_DefaultData = StormReflGetDefault<DataClass>();
  }

  template <typename C>
  void RegisterCallback(void(C::*func)(uint32_t, DataClass &&), C * c)
  {
    auto callback_func = [=](uint32_t seq, DataClass && inst) { (c->*func)(seq, std::move(inst)); };
    RegisterCallback(callback_func);
  }

  void SetDefault(DataClass * default_data_ptr)
  {
    m_DefaultData = default_data_ptr;
  }

  void RegisterCallback(std::function<void(uint32_t, DataClass &&)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  template <typename C>
  void RegisterCallback(void(C::*func)(uint32_t, const DataClass &), C * c)
  {
    auto callback_func = [=](uint32_t seq, const DataClass & inst) { (c->*func)(seq, inst); };
    RegisterCallback(callback_func);
  }

  void GotMessage(NetBitReader & reader)
  {
    if (m_DefaultData == nullptr)
    {
      return;
    }

    DataClass inst = *m_DefaultData;
    uint32_t seq;
    NetDeserializeValue(seq, reader);
    NetDeserializeValue(inst, reader);

    if (m_UpdateCallback)
    {
      m_UpdateCallback(seq, std::move(inst));
    }
  }

private:

  std::function<void(uint32_t, DataClass &&)> m_UpdateCallback;
  DataClass * m_DefaultData;
};

template <class DataClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeFullStateSeq
{
  using SenderType = NetPipeFullStateSeqSender<DataClass>;
  using ReceiverType = NetPipeFullStateSeqReciever<DataClass>;

  static const NetPipeMode PipeMode = Mode;
};
=======
#pragma once

#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetTransmitter.h"

class NetBitWriter;
class NetBitReader;

template <class DataClass>
class NetPipeFullStateSeqSender
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  void SyncState(uint32_t seq, const DataClass & state)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);
    writer.WriteBits(seq, 32);
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
class NetPipeFullStateSeqReciever
{
public:
  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_DefaultData = StormReflGetDefault<DataClass>();
  }

  template <typename C>
  void RegisterCallback(void(C::*func)(uint32_t, DataClass &&), C * c)
  {
    auto callback_func = [=](uint32_t seq, DataClass && inst) { (c->*func)(seq, std::move(inst)); };
    RegisterCallback(callback_func);
  }

  void SetDefault(DataClass * default_data_ptr)
  {
    m_DefaultData = default_data_ptr;
  }

  void RegisterCallback(std::function<void(uint32_t, DataClass &&)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  template <typename C>
  void RegisterCallback(void(C::*func)(uint32_t, const DataClass &), C * c)
  {
    auto callback_func = [=](uint32_t seq, const DataClass & inst) { (c->*func)(seq, inst); };
    RegisterCallback(callback_func);
  }

  void GotMessage(NetBitReader & reader)
  {
    if (m_DefaultData == nullptr)
    {
      return;
    }

    DataClass inst = *m_DefaultData;
    uint32_t seq;
    NetDeserializeValue(seq, reader);
    NetDeserializeValue(inst, reader);

    if (m_UpdateCallback)
    {
      m_UpdateCallback(seq, std::move(inst));
    }
  }

private:

  std::function<void(uint32_t, DataClass &&)> m_UpdateCallback;
  DataClass * m_DefaultData;
};

template <class DataClass, NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeFullStateSeq
{
  using SenderType = NetPipeFullStateSeqSender<DataClass>;
  using ReceiverType = NetPipeFullStateSeqReciever<DataClass>;

  static const NetPipeMode PipeMode = Mode;
};
>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
