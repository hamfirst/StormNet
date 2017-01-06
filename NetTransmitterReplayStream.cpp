
#include "NetTransmitterReplayStream.h"


NetBitWriter & NetTransmitterReplayStream::CreateWriter(NetPipeMode mode)
{
  return m_Writer;
}

void NetTransmitterReplayStream::SendWriter(NetBitWriter & writer)
{

}
