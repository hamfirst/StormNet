<<<<<<< HEAD

#include "NetReflectionBitBuffer.h"


NetReflectionBitBufferReader::NetReflectionBitBufferReader(NetBitWriterVector & data_writer) :
  m_DataReader(data_writer.GetData(), data_writer.GetDataSize())
{

}

void NetReflectionBitBuffer::Reset()
{
  m_DataWriter.Reset();
}

NetReflectionBitBufferReader NetReflectionBitBuffer::CreateReader()
{
  return NetReflectionBitBufferReader(m_DataWriter);
}
=======

#include "NetReflectionBitBuffer.h"


NetReflectionBitBufferReader::NetReflectionBitBufferReader(NetBitWriterVector & data_writer) :
  m_DataReader(data_writer.GetData(), data_writer.GetDataSize())
{

}

void NetReflectionBitBuffer::Reset()
{
  m_DataWriter.Reset();
}

NetReflectionBitBufferReader NetReflectionBitBuffer::CreateReader()
{
  return NetReflectionBitBufferReader(m_DataWriter);
}
>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
