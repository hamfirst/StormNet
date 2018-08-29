<<<<<<< HEAD
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


=======
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


>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
