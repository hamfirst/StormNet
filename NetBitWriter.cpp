
#include "NetBitWriter.h"

void NetBitWriter::WriteBuffer(void * data, std::size_t num_bytes)
{
  uint64_t * ptr = (uint64_t *)data;

  while (num_bytes >= 8)
  {
    WriteBits(*ptr, 64);
    ptr++;
    num_bytes -= 8;
  }

  uint8_t * byte_ptr = (uint8_t *)ptr;
  while (num_bytes > 0)
  {
    WriteBits(*byte_ptr, 8);
    byte_ptr++;
    num_bytes--;
  }
}
