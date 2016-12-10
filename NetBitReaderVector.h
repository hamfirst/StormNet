#pragma once

#include <vector>

#include "NetBitReaderBuffer.h"
#include "NetBitWriterVector.h"

class NetBitReaderVector : public NetBitReaderBuffer
{
public:
  NetBitReaderVector(std::vector<uint8_t> && data);
  NetBitReaderVector(NetBitWriterVector && data);

  NetBitReaderVector(const NetBitReaderVector & rhs) = delete;
  NetBitReaderVector(NetBitReaderVector && rhs);

  NetBitReaderVector & operator = (const NetBitReaderVector & rhs) = delete;
  NetBitReaderVector & operator = (NetBitReaderVector && rhs);


private:
  std::vector<uint8_t> m_Vector;
};
