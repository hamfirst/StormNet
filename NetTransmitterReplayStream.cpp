
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "NetTransmitterReplayStream.h"


NetTransmitterReplayStream::NetTransmitterReplayStream(NetReplayTimerFunc time_callback)
{
  m_InitialBlock.m_Next = nullptr;
  m_CurrentBlock = &m_InitialBlock;
  m_CurrentOffset = 0;

  m_TimeCallback = time_callback;
  m_StartTime = m_TimeCallback();
}

NetTransmitterReplayStream::~NetTransmitterReplayStream()
{
  Clear();
}

NetBitWriter & NetTransmitterReplayStream::CreateWriter(NetPipeMode mode, int channel_index, int channel_bits, bool ack)
{
  CreateDefaultMessage(m_Writer, channel_index, channel_bits, ack);
  return m_Writer;
}

void NetTransmitterReplayStream::SendWriter(NetBitWriter & writer)
{
  auto ptr = (uint8_t *)m_Writer.GetData();
  uint32_t size = (uint32_t)m_Writer.GetDataSize();

  auto time = m_TimeCallback() - m_StartTime;
  WriteData((uint8_t *)&time, sizeof(time));
  WriteData((uint8_t *)&size, sizeof(size));
  WriteData(ptr, size);

  m_Writer.Reset();
}


void NetTransmitterReplayStream::Start()
{
  m_StartTime = m_TimeCallback();
}

bool NetTransmitterReplayStream::SaveToFile(const char * file_name)
{
  FILE * fp = fopen(file_name, "wb");

  if (fp == nullptr)
  {
    return false;
  }

  auto block = &m_InitialBlock;
  while (block != nullptr)
  {
    if (block == m_CurrentBlock)
    {
      fwrite(&block->m_Buffer[0], m_CurrentOffset, 1, fp);
    }
    else
    {
      fwrite(&block->m_Buffer[0], kNetTransmitterBufferSize, 1, fp);
    
    }

    block = block->m_Next;
  }

  fclose(fp);
  return true;
}

void NetTransmitterReplayStream::Clear()
{
  auto block = m_InitialBlock.m_Next;
  while (block != nullptr)
  {
    auto dead_block = block;
    block = block->m_Next;

    free(dead_block);
  }

  m_InitialBlock.m_Next = nullptr;
  m_CurrentBlock = &m_InitialBlock;
  m_CurrentOffset = 0;
}

void NetTransmitterReplayStream::WriteData(uint8_t * ptr, std::size_t size)
{
  while (size > 0)
  {
    std::size_t space_avail = kNetTransmitterBufferSize - m_CurrentOffset;

    if (space_avail >= size)
    {
      memcpy(&m_CurrentBlock->m_Buffer[m_CurrentOffset], ptr, size);
      m_CurrentOffset += (int)size;
      break;
    }
    else
    {
      memcpy(&m_CurrentBlock->m_Buffer[m_CurrentOffset], ptr, space_avail);
      ptr += space_avail;
      size -= space_avail;

      m_CurrentOffset = 0;
      m_CurrentBlock->m_Next = (NetTransmitterBlock *)malloc(sizeof(NetTransmitterBlock));
      m_CurrentBlock = m_CurrentBlock->m_Next;
      m_CurrentBlock->m_Next = nullptr;
    }
  }
}
