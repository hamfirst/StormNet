#pragma once

#include <cstddef>
#include <cstdint>

class NetRandom
{
public:

  NetRandom();
  NetRandom(uint32_t seed);

  NetRandom(const NetRandom & rhs) = default;
  NetRandom(NetRandom && rhs) = default;
  NetRandom & operator = (const NetRandom & rhs) = default;
  NetRandom & operator = (NetRandom && rhs) = default;

  uint32_t GetRandom();
  uint64_t GetRandom64();

  uint32_t GetSeed() const;

protected:

  uint32_t m_State;
};


