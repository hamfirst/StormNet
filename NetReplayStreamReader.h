#pragma once

#include "NetTransmitterNull.h"
#include "NetProtocol.h"
#include "NetBitReaderBuffer.h"

typedef unsigned int(*NetReplayPlaybackTimerFunc)();

class NetReplayStreamCursor
{
  uint32_t m_CurrentTime;

  std::size_t m_RemainingSize;

  uint32_t m_NextPacketSize;
  uint32_t m_NextPacketTime;

  template <class ProtocolDef>
  friend class NetReplayStreamReader;
};

template <class ProtocolDef>
class NetReplayStreamReader
{
public:

  using ProtocolType = typename NetProtocolInfo<ProtocolDef>::SymmetricProtocolType;

  NetReplayStreamReader(NetReplayPlaybackTimerFunc timer_func, void * buffer, std::size_t size) :
    m_TimerFunc(timer_func),
    m_Buffer((uint8_t *)buffer),
    m_RemainingSize(size),
    m_Protocol(&m_Transmitter)
  {
    m_StartTime = m_TimerFunc();
    ReadNextPacket();
  }

  void Update()
  {
    auto cur_time = m_TimerFunc() - m_StartTime;
    while (m_NextPacketTime <= cur_time)
    {
      NetBitReaderBuffer buffer(m_Buffer, m_NextPacketSize);
      m_Buffer += m_NextPacketSize;

      m_Protocol.GotMessage(buffer);
      ReadNextPacket();
    }
  }

  void SkiptToNextPacket()
  {
    if (m_NextPacketTime == INT_MAX)
    {
      return;
    }

    auto cur_time = m_TimerFunc() - m_StartTime;
    if (m_NextPacketTime > cur_time)
    {
      m_StartTime -= m_NextPacketTime - cur_time;
    }
  }

  bool IsFinished()
  {
    return m_NextPacketTime == INT_MAX;
  }

  auto & GetProtocol()
  {
    return m_Protocol;
  }

  NetReplayStreamCursor GetCursor()
  {
    NetReplayStreamCursor cursor;
    cursor.m_CurrentTime = m_TimerFunc() - m_StartTime;
    cursor.m_NextPacketSize = m_NextPacketSize;
    cursor.m_NextPacketTime = m_NextPacketTime;
    cursor.m_RemainingSize = m_RemainingSize;
    return cursor;
  }

  void LoadCursor(const NetReplayStreamCursor & cursor)
  {
    NetReplayStreamCursor cursor;
    m_StartTime = m_TimerFunc() - cursor.m_CurrentTime;
    m_NextPacketSize = cursor.m_NextPacketSize;
    m_NextPacketTime = cursor.m_NextPacketTime;
    m_RemainingSize = cursor.m_RemainingSize;
    return cursor;
  }

private:

  void ReadNextPacket()
  {
    if (m_RemainingSize < sizeof(uint32_t) * 2)
    {
      m_NextPacketSize = 0;
      m_NextPacketTime = INT_MAX;
      return;
    }

    uint32_t time = *((uint32_t *)m_Buffer);
    m_Buffer += sizeof(uint32_t);
    uint32_t size = *((uint32_t *)m_Buffer);
    m_Buffer += sizeof(uint32_t);

    if (m_RemainingSize < size)
    {
      m_NextPacketSize = 0;
      m_NextPacketTime = INT_MAX;
      return;
    }

    m_NextPacketSize = size;
    m_NextPacketTime = time;
  }

private:

  uint32_t m_StartTime;

  uint8_t * m_Buffer;
  std::size_t m_RemainingSize;

  uint32_t m_NextPacketSize;
  uint32_t m_NextPacketTime;

  NetReplayPlaybackTimerFunc m_TimerFunc;

  NetTransmitterNull m_Transmitter;
  ProtocolType m_Protocol;
};

