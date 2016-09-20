#pragma once

#include <cstdint>
#include <type_traits>

namespace ClzHelper
{
  // 64 bit clz
  constexpr unsigned Clz1_64(uint64_t x)
  {
    return (x <= 0x7fffffffffffffffULL ? 1 : 0);
  }

  constexpr unsigned Clz2_64(uint64_t x)
  {
    return (x <= 0x3fffffffffffffffULL ? 2 + Clz1_64(x << 2) : Clz1_64(x));
  }

  constexpr unsigned Clz4_64(uint64_t x)
  {
    return (x <= 0x0fffffffffffffffULL ? 4 + Clz2_64(x << 4) : Clz2_64(x));
  }

  constexpr unsigned Clz8_64(uint64_t x)
  {
    return (x <= 0x00ffffffffffffffULL ? 8 + Clz4_64(x << 8) : Clz4_64(x));
  }

  constexpr unsigned Clz16_64(uint64_t x)
  {
    return (x <= 0x0000ffffffffffffULL ? 16 + Clz8_64(x << 16) : Clz8_64(x));
  }

  constexpr unsigned Clz32_64(uint64_t x)
  {
    return (x <= 0x00000000ffffffffULL ? 32 + Clz16_64(x << 32) : Clz16_64(x));
  }


  // 32 bit clz
  constexpr unsigned Clz1_32(uint32_t x)
  {
    return (x <= 0x7fffffff ? 1 : 0);
  }

  constexpr unsigned Clz2_32(uint32_t x)
  {
    return (x <= 0x3fffffff ? 2 + Clz1_32(x << 2) : Clz1_32(x));
  }

  constexpr unsigned Clz4_32(uint32_t x)
  {
    return (x <= 0x0fffffff ? 4 + Clz2_32(x << 4) : Clz2_32(x));
  }

  constexpr unsigned Clz8_32(uint32_t x)
  {
    return (x <= 0x00ffffff ? 8 + Clz4_32(x << 8) : Clz4_32(x));
  }

  constexpr unsigned Clz16_32(uint32_t x)
  {
    return (x <= 0x0000ffff ? 16 + Clz8_32(x << 16) : Clz8_32(x));
  }


  // 16 bit clz
  constexpr unsigned Clz1_16(uint16_t x)
  {
    return (x <= 0x7fff ? 1 : 0);
  }

  constexpr unsigned Clz2_16(uint16_t x)
  {
    return (x <= 0x3fff ? 2 + Clz1_16(x << 2) : Clz1_16(x));
  }

  constexpr unsigned Clz4_16(uint16_t x)
  {
    return (x <= 0x0fff ? 4 + Clz2_16(x << 4) : Clz2_16(x));
  }

  constexpr unsigned Clz8_16(uint16_t x)
  {
    return (x <= 0x00ff ? 8 + Clz4_16(x << 8) : Clz4_16(x));
  }


  // 8 bit clz
  constexpr unsigned Clz1_8(uint8_t x)
  {
    return (x <= 0x7f ? 1 : 0);
  }

  constexpr unsigned Clz2_8(uint8_t x)
  {
    return (x <= 0x3f ? 2 + Clz1_8(x << 2) : Clz1_8(x));
  }

  constexpr unsigned Clz4_8(uint8_t x)
  {
    return (x <= 0x0f ? 4 + Clz2_8(x << 4) : Clz2_8(x));
  }
}

template <typename NumericType>
constexpr unsigned CountLeadingZeros(NumericType val)
{
  return  (val < 0) ? 0 : ClzHelper::Clz16_32(static_cast<uint32_t>(val));
}

template <>
constexpr unsigned CountLeadingZeros(uint64_t val)
{
  return ClzHelper::Clz32_64(val);
}

template <>
constexpr unsigned CountLeadingZeros(int64_t val)
{
  return val < 0 ? 0 : ClzHelper::Clz32_64(val);
}

template <>
constexpr unsigned CountLeadingZeros(uint32_t x)
{
  return ClzHelper::Clz16_32(x);
}

template <>
constexpr unsigned CountLeadingZeros(uint16_t x)
{
  return ClzHelper::Clz8_16(x);
}

template <>
constexpr unsigned CountLeadingZeros(uint8_t x)
{
  return ClzHelper::Clz4_8(x);
}

template <class NumericType>
constexpr unsigned GetRequiredBits(NumericType max_value)
{
  return sizeof(NumericType) * 8 - CountLeadingZeros(max_value);
}

template <uint64_t MaxVal>
class MinStorageRequired
{
public:
  static const bool bigger_than_32 = GetRequiredBits(MaxVal) > 32;
  static const bool bigger_than_16 = GetRequiredBits(MaxVal) > 16;
  static const bool bigger_than_8 = GetRequiredBits(MaxVal) > 8;

  using type = std::conditional_t<bigger_than_32, uint64_t,
               std::conditional_t<bigger_than_16, uint32_t,
               std::conditional_t<bigger_than_8, uint16_t, uint8_t>>>;
};

template <uint64_t MaxVal>
using min_store_t = typename MinStorageRequired<MaxVal>::type;
