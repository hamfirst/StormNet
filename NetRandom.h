#pragma once

#include <cstddef>
#include <cstdint>

class NetRandom
{
public:

  NetRandom();
  NetRandom(uint32_t seed);

  uint32_t GetRandom();
  uint64_t GetRandom64();

private:

  uint32_t m_State;
};


