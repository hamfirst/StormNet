
#include "NetBitReaderVector.h"

NetBitReaderVector::NetBitReaderVector(std::vector<uint8_t> && data) :
  NetBitReaderBuffer(nullptr, 0),
  m_Vector(std::move(data))
{
  m_Buffer = m_Vector.data();
  m_NumBytes = m_Vector.size();
}

NetBitReaderVector::NetBitReaderVector(NetBitWriterVector && data) :
  NetBitReaderVector(std::move(data.m_Buffer))
{
  data.Reset();
}

NetBitReaderVector::NetBitReaderVector(NetBitReaderVector && rhs) : 
  NetBitReaderVector(std::move(rhs.m_Vector))
{
  m_Bit = rhs.m_Bit;
  m_Offset = rhs.m_Offset;

  rhs.m_Buffer = nullptr;
  rhs.m_NumBytes = 0;
  rhs.m_Offset = 0;
  rhs.m_Bit = 0;
}

NetBitReaderVector & NetBitReaderVector::operator = (NetBitReaderVector && rhs)
{
  m_Vector = std::move(rhs.m_Vector);
  m_Buffer = m_Vector.data();
  m_NumBytes = m_Vector.size();
  m_Bit = rhs.m_Bit;
  m_Offset = rhs.m_Offset;

  rhs.m_Buffer = nullptr;
  rhs.m_NumBytes = 0;
  rhs.m_Offset = 0;
  rhs.m_Bit = 0;

  return *this;
}

