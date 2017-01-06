#pragma once

#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetTransmitter.h"

class NetBitWriter;
class NetBitReader;

class NetPipeBufferSender
{
public:

  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {
    m_Transmitter = transmitter;
    m_Mode = mode;
    m_ChannelIndex = channel_index;
    m_ChannelBits = channel_bits;
  }

  void SendData(void * data, std::size_t size)
  {
    NetBitWriter & writer = m_Transmitter->CreateMessage(m_Mode, m_ChannelIndex, m_ChannelBits);
    writer.WriteBits(size, 32);
    writer.WriteBuffer(data, size);
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

class NetPipeBufferReciever
{
public:
  void Initialize(NetTransmitter * transmitter, NetPipeMode mode, int channel_index, int channel_bits)
  {

  }

  template <typename C>
  void RegisterCallback(void(C::*func)(void *, std::size_t), C * c)
  {
    auto callback_func = [=](void * data, std::size_t size) { (c->*func)(data, size); };
    RegisterCallback(callback_func);
  }

  void RegisterCallback(std::function<void(void *, std::size_t)> && func)
  {
    m_UpdateCallback = std::move(func);
  }

  void GotMessage(NetBitReader & reader)
  {
    std::size_t size = (std::size_t)reader.ReadUBits(32);

    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(size);
    reader.ReadBuffer(buffer.get(), size);

    if (m_UpdateCallback)
    {
      m_UpdateCallback(buffer.get(), size);
    }
  }

private:

  std::function<void(void *, std::size_t)> m_UpdateCallback;
};


template <NetPipeMode Mode = NetPipeMode::kReliable>
struct NetPipeBuffer
{
  using SenderType = NetPipeBufferSender;

  using ReceiverType = NetPipeBufferReciever;

  static const NetPipeMode PipeMode = Mode;
};
