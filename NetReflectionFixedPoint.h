#pragma once

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetException.h"

#ifndef _WEB
#include <xmmintrin.h>
#endif

#pragma warning(push)
#pragma warning(disable : 4293)

inline int64_t FixedPointMultiplyWide(int64_t a, int64_t b, int FractionalBits)
{
#if !defined(WIN32) && !defined(_LINUX) && !defined(_WEB) && !defined(STORM_REFL_PARSE) && defined(SHIT)

  int64_t hi;
  int64_t result = _mul128(a, b, &hi);

  int shift_bits = FractionalBits;
  int result_bits = 64 - shift_bits;

  uint64_t result_mask = (1ULL << result_bits) - 1;

  result >>= shift_bits;
  result &= result_mask;
  result |= (hi << result_bits);
  return result;
#else

  auto xl = a;
  auto yl = b;

  auto mask = (uint64_t)(1ULL << FractionalBits) - 1;

  auto xlo = (uint64_t)(xl & mask);
  auto xhi = xl >> FractionalBits;
  auto ylo = (uint64_t)(yl & mask);
  auto yhi = yl >> FractionalBits;

  auto lolo = xlo * ylo;
  auto lohi = (int64_t)xlo * yhi;
  auto hilo = xhi * (int64_t)ylo;
  auto hihi = xhi * yhi;

  auto loResult = lolo >> FractionalBits;
  auto midResult1 = lohi;
  auto midResult2 = hilo;
  auto hiResult = hihi << FractionalBits;

  auto sum = (int64_t)loResult + midResult1 + midResult2 + hiResult;
  return sum;

#endif
}

inline int32_t FixedPointMultiplyWide(int32_t a, int32_t b, int FractionalBits)
{
  return (int32_t)(((int64_t)a * (int64_t)b) >> (FractionalBits));
}

inline int16_t FixedPointMultiplyWide(int16_t a, int16_t b, int FractionalBits)
{
  return (int16_t)(((int32_t)a * (int32_t)b) >> (FractionalBits));
}

inline int8_t FixedPointMultiplyWide(int8_t a, int8_t b, int FractionalBits)
{
  return (int8_t)(((int16_t)a * (int16_t)b) >> (FractionalBits));
}

inline int64_t FixedPointDivideWide(int64_t a, int64_t b, int FractionalBits)
{
  auto xl = a;
  auto yl = b;

  if (yl == 0) 
  {
    NET_THROW_OR(std::logic_error("Divide by zero"), return 0);
  }

  int64_t max_value = std::numeric_limits<int64_t>::max();
  int64_t min_value = std::numeric_limits<int64_t>::min();

  uint64_t remainder = (uint64_t)(xl >= 0 ? xl : -xl);
  uint64_t divider = (uint64_t)(yl >= 0 ? yl : -yl);
  uint64_t quotient = 0ULL;
  int bit_pos = FractionalBits + 1;

  // If the divider is divisible by 2^n, take advantage of it.
  while ((divider & 0xF) == 0 && bit_pos >= 4) 
  {
    divider >>= 4;
    bit_pos -= 4;
  }

  while (remainder != 0 && bit_pos >= 0)
  {
    int shift = CountLeadingZeros(remainder);
    if (shift > bit_pos) 
    {
      shift = bit_pos;
    }

    remainder <<= shift;
    bit_pos -= shift;

    auto div = remainder / divider;
    remainder = remainder % divider;
    quotient += div << bit_pos;

    // Detect overflow
    //if ((div & ~(0xFFFFFFFFFFFFFFFF >> bit_pos)) != 0) 
    //{
    //  if (((xl ^ yl) & max_value) == 0)
    //  {
    //    return MaxValue;
    //  }
    //  else
    //  {
    //    return MinValue;
    //  }
    //}

    remainder <<= 1;
    --bit_pos;
  }

  // rounding
  ++quotient;
  auto result = (long)(quotient >> 1);
  if (((xl ^ yl) & min_value) != 0)
  {
    result = -result;
  }

  return result;
}

inline int32_t FixedPointDivideWide(int32_t a, int32_t b, int FractionalBits)
{
  return (int32_t)(((int64_t)a << FractionalBits) / ((int64_t)b));
}

inline int16_t FixedPointDivideWide(int16_t a, int16_t b, int FractionalBits)
{
  return (int16_t)(((int32_t)a << FractionalBits) / ((int32_t)b));
}

inline int8_t FixedPointDivideWide(int8_t a, int8_t b, int FractionalBits)
{
  return (int8_t)(((int16_t)a << FractionalBits) / ((int16_t)b));
}

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
class NetFixedPoint
{
public:
  using FixedType = NetFixedPoint<StorageType, NumBits, FractionalBits>;

  constexpr NetFixedPoint() = default;
  constexpr NetFixedPoint(const FixedType & rhs) = default;
  constexpr NetFixedPoint(FixedType && rhs) = default;

  FixedType & operator = (const FixedType & rhs) = default;
  FixedType & operator = (FixedType && rhs) = default;

  constexpr NetFixedPoint(int8_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  constexpr NetFixedPoint(int16_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  constexpr NetFixedPoint(int32_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  constexpr NetFixedPoint(int64_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  explicit NetFixedPoint(const char * str)
  {
    bool negative = false;
    if (*str == '-')
    {
      str++;
      negative = true;
    }

    if (*str == '+')
    {
      str++;
    }

    StorageType val = 0;
    while (true)
    {
      if (*str < '0' || *str > '9')
      {
        break;
      }

      val *= 10;
      val += *str - '0';

      str++;
    }

    val <<= FractionalBits;

    if (*str == '.')
    {
      str++;
      StorageType dividend = 1;

      while (true)
      {
        if (*str < '0' || *str > '9')
        {
          break;
        }

        auto frac = *str - '0';
        frac <<= FractionalBits;

        dividend *= 10;
        val += (frac + (dividend >> 2)) / dividend;

        str++;
      }
    }

    m_Value = val;

    if (negative)
    {
      m_Value = -m_Value;
    }
  }

  static FixedType CreateFromRawVal(StorageType val)
  {
    FixedType f;
    f.m_Value = val;
    return f;
  }

  static FixedType Epsilon()
  {
    return CreateFromRawVal(1);
  }

  static FixedType CreateFromFloat(float val)
  {
    FixedType f;
    f.m_Value = (StorageType)((float)kOne * val);
    return f;
  }

  StorageType GetRawVal() const
  {
    return m_Value;
  }

  operator bool() const
  {
    return m_Value != 0;
  }

  operator float () const
  {
    return (float)m_Value / (float)kOne;
  }

  operator int() const
  {
    return (int)(m_Value >> FractionalBits);
  }

  FixedType operator ++()
  {
    Set(m_Value + kOne);
    return CreateFromRawVal(m_Value);
  }

  FixedType operator --()
  {
    Set(m_Value - kOne);
    return CreateFromRawVal(m_Value);
  }

  FixedType operator !() const
  {
    return m_Value ? CreateFromRawVal(0) : CreateFromRawVal(kOne);
  }

  FixedType operator + (FixedType val) const
  {
    return CreateFromRawVal(m_Value + val.m_Value);
  }

  FixedType operator += (FixedType val)
  {
    Set(m_Value + val.m_Value);
    return CreateFromRawVal(m_Value);
  }

  FixedType operator - (FixedType val) const
  {
    return CreateFromRawVal(m_Value - val.m_Value);
  }

  FixedType operator -= (FixedType val)
  {
    Set(m_Value - val.m_Value);
    return CreateFromRawVal(m_Value);
  }

  FixedType operator * (FixedType val) const
  {
    return CreateFromRawVal(FixedPointMultiplyWide(m_Value, val.m_Value, FractionalBits));
  }

  FixedType operator *= (FixedType val)
  {
    auto result = this->operator *(val);
    Set(result.m_Value);
    return CreateFromRawVal(m_Value);
  }

  FixedType operator / (FixedType val) const
  {
    return CreateFromRawVal(FixedPointDivideWide(m_Value, val.m_Value, FractionalBits));
  }

  FixedType operator /= (FixedType val)
  {
    auto result = this->operator /(val);
    Set(result.m_Value);
    return CreateFromRawVal(m_Value);
  }

  bool operator == (FixedType val) const
  {
    return m_Value == val.m_Value;
  }

  bool operator != (FixedType val) const
  {
    return m_Value != val.m_Value;
  }

  bool operator > (FixedType val) const
  {
    return m_Value > val.m_Value;
  }

  bool operator >= (FixedType val) const
  {
    return m_Value >= val.m_Value;
  }

  bool operator < (FixedType val) const
  {
    return m_Value < val.m_Value;
  }

  bool operator <= (FixedType val) const
  {
    return m_Value <= val.m_Value;
  }

  FixedType operator -() const
  {
    return Invert();
  }

  FixedType Invert() const
  {
    return CreateFromRawVal(-m_Value);
  }

  FixedType Abs() const
  {
    return CreateFromRawVal(m_Value > 0 ? m_Value : -m_Value);
  }

  FixedType Frac() const
  {
    return CreateFromRawVal(m_Value & kFractionalMask);
  }

  FixedType Floor() const
  {
    auto f = Frac();
    return CreateFromRawVal(m_Value - f.m_Value);
  }

  FixedType Round() const
  {
    auto frac = m_Value & (kFractionalMask);
    auto val = m_Value & (~kFractionalMask);

    StorageType pos = 1;
    pos <<= (FractionalBits - 1);

    if (frac > pos)
    {
      return CreateFromRawVal(val + kOne);
    }
    else
    {
      return CreateFromRawVal(val);
    }
  }

  FixedType Ceil() const
  {
    auto f = Floor();
    return f.m_Value != m_Value ? CreateFromRawVal(f.m_Value + kOne) : CreateFromRawVal(m_Value);
  }

  int Clz() const
  {
    auto byte_mask = 0xF << (NumBits - 4);
    auto bit_mask = 0x1 << (NumBits - 1);

    int bits = 0;
    auto val = m_Value;

    while ((val & byte_mask) == 0 && bits < 32) 
    { 
      bits += 4; 
      val <<= 4; 
    }
    
    while ((val & bit_mask) == 0 && bits < 8) 
    { 
      bits += 1; 
      val <<= 1; 
    }

    return bits;
  }

  FixedType Min(const FixedType & m) const
  {
    return m_Value <= m.m_Value ? CreateFromRawVal(m_Value) : CreateFromRawVal(m.m_Value);
  }

  FixedType Max(const FixedType & m) const
  {
    return m_Value >= m.m_Value ? CreateFromRawVal(m_Value) : CreateFromRawVal(m.m_Value);
  }

  void Clamp(const FixedType & min, const FixedType & max)
  {
    if(m_Value < min.m_Value)
    {
      m_Value = min.m_Value;
    }
    else if (m_Value > max.m_Value)
    {
      m_Value = max.m_Value;
    }
  }

  FixedType Sqrt() const
  {
    if (m_Value <= 0)
    {
      return CreateFromRawVal(0);
    }

    auto zeros = Clz();
    int guess_bits = (FractionalBits + (NumBits - zeros)) / 2;

    auto guess = StorageType(1);
    guess <<= guess_bits;

    if (guess == 0)
    {
      guess = kOne;
    }

    auto guess_fixed = CreateFromRawVal(guess);

    for (int i = 0; i < 4; i++)
    {
      auto div = *this / guess_fixed;
      auto avg = (div.m_Value >> 1) + (guess_fixed.m_Value >> 1);

      if (avg == guess_fixed.m_Value)
      {
        break;
      }

      guess_fixed = CreateFromRawVal(avg);
    }

    return guess_fixed;
  }

  FixedType SinSlow() const
  {
    auto one = CreateFromRawVal(kOne);
    auto val = CreateFromRawVal(m_Value);
    auto x = CreateFromRawVal(m_Value);

    auto div = one;
    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^3/3!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^5/5!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^7/7!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^9/9!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^11/11!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^13/13!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^15/15!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^17/17!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^19/19!
    return val;
  }

  FixedType CosSlow() const
  {
    auto one = CreateFromRawVal(kOne);
    auto val = CreateFromRawVal(kOne);
    auto x = CreateFromRawVal(m_Value);

    x.m_Value >>= 1;
    x *= *this;

    val -= x; // -x^2/2!

    x *= *this;
    x.m_Value >>= 2;

    auto div = one + one + one;

    x *= *this;
    x /= div;

    val += x; // +x^4/4!

    div += one + one;

    x *= *this;
    x /= div;
    
    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^6/6!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^8/8!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^10/10!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^12/12!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^14/14!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val += x; // +x^16/16!

    div += one;
    x *= *this;
    x /= div;

    div += one;
    x *= *this;
    x /= div;

    val -= x; // -x^18/18!
    return val;
  }

  FixedType AtanSlow() const
  {
    auto one = CreateFromRawVal(kOne);
    auto two = one + one;
    auto val = CreateFromRawVal(m_Value);
    auto x = CreateFromRawVal(m_Value);

    auto div = one;
    div += two;
    x *= *this;
    x *= *this;
    val -= x / div; // -x^3/3

    div += two;
    x *= *this;
    x *= *this;
    val += x / div; // +x^5/5

    div += two;
    x *= *this;
    x *= *this;
    val -= x / div; // -x^7/7

    div += two;
    x *= *this;
    x *= *this;
    val += x / div; // +x^9/9

    div += two;
    x *= *this;
    x *= *this;
    val -= x / div; // -x^11/11

    div += two;
    x *= *this;
    x *= *this;
    val += x / div; // +x^13/13

    div += two;
    x *= *this;
    x *= *this;
    val -= x / div; // -x^15/15

    div += two;
    x *= *this;
    x *= *this;
    val += x / div; // +x^17/17

    div += two;
    x *= *this;
    x *= *this;
    val -= x / div; // -x^19/19

    div += two;
    x *= *this;
    x *= *this;
    val += x / div; // +x^21/21

    div += two;
    x *= *this;
    x *= *this;
    val -= x / div; // -x^23/23
    return val;
  }


  static FixedType RandomRange(const FixedType & range_low, const FixedType & range_high, StorageType rand_val)
  {
    auto diff = range_high.m_Value - range_low.m_Value;
    if (diff == 0)
    {
      return range_low;
    }

    return CreateFromRawVal(range_low.m_Value + (rand_val % diff));
  }

private:

  void Set(StorageType val)
  {
    m_Value = (val & kMask);
  }

  template <typename LutStorageType, LutStorageType LutNumBits, LutStorageType LutFractionalBits>
  friend class NetFixedPointLUT;

  static const StorageType kMask = NumBits >= sizeof(StorageType) ? StorageType(-1) : ((StorageType(1) << NumBits) - 1);
  static const StorageType kFractionalMask = (StorageType(1) << FractionalBits) - 1;
  static const StorageType kDecimalMask = (~kFractionalMask) & kMask;

  static const StorageType kOne = StorageType(1) << FractionalBits;

  StorageType m_Value = 0;
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits, class NetBitWriter>
struct NetSerializer<NetFixedPoint<StorageType, NumBits, FractionalBits>, NetBitWriter>
{
  void operator()(const NetFixedPoint<StorageType, NumBits, FractionalBits> & val, NetBitWriter & writer)
  {
    if (std::is_unsigned<StorageType>::value)
    {
      writer.WriteBits(val.GetRawVal(), NumBits);
    }
    else
    {
      writer.WriteSBits(val.GetRawVal(), NumBits);
    }
  }
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits, class NetBitReader>
struct NetDeserializer<NetFixedPoint<StorageType, NumBits, FractionalBits>, NetBitReader>
{
  void operator()(NetFixedPoint<StorageType, NumBits, FractionalBits> & val, NetBitReader & reader)
  {
    if (std::is_unsigned<StorageType>::value)
    {
      StorageType raw_val = static_cast<StorageType>(reader.ReadUBits(NumBits));
      val = NetFixedPoint<StorageType, NumBits, FractionalBits>::CreateFromRawVal(raw_val);
    }
    else
    {
      StorageType raw_val = static_cast<StorageType>(reader.ReadSBits(NumBits));
      val = NetFixedPoint<StorageType, NumBits, FractionalBits>::CreateFromRawVal(raw_val);
    }
  }
};



#pragma warning(pop)