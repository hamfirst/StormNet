#pragma once

#include <type_traits>
#include <limits>

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"

template <class NumericType, NumericType Min = std::numeric_limits<NumericType>::min, NumericType Max = std::numeric_limits<NumericType>::max>
class NetRangedNumber
{
public:

  NetRangedNumber()
  {
    m_Value = std::max(Min, std::min(Max, 0));
  }

  NetRangedNumber(NumericType val)
  {
    m_Value = std::max(Min, std::min(Max, val));
  }

  NetRangedNumber(const NetRangedNumber<NumericType, Min, Max> & val)
  {
    m_Value = val.m_Value;
  }

  template <class OtherNumericType, OtherNumericType OtherMin, OtherNumericType OtherMax>
  NetRangedNumber(const NetRangedNumber<OtherNumericType, OtherMin, OtherMax> & val)
  {
    m_Value = std::max(Min, std::min(Max, static_cast<NumericType>(val)));
  }

  NumericType operator = (NumericType val)
  {
    Set(val);
    return m_Value;
  }

  operator NumericType() const
  {
    return m_Value;
  }

  operator bool() const
  {
    return m_Value != 0;
  }

  float AsFloat() const
  {
    return (float)m_Value;
  }

  NumericType operator ++()
  {
    Set(m_Value + 1);
    return m_Value;
  }

  NumericType operator --()
  {
    Set(m_Value - 1);
    return m_Value;
  }

  NumericType operator !() const
  {
    return !m_Value;
  }

  NumericType operator + (NumericType val) const
  {
    return m_Value + val;
  }

  NumericType operator += (NumericType val)
  {
    Set(m_Value + val);
    return m_Value;
  }

  NumericType operator - (NumericType val) const
  {
    return m_Value - val;
  }

  NumericType operator -= (NumericType val)
  {
    Set(m_Value - val);
    return m_Value;
  }

  NumericType operator * (NumericType val) const
  {
    return m_Value * val;
  }

  NumericType operator *= (NumericType val)
  {
    Set(m_Value * val);
    return m_Value;
  }

  NumericType operator / (NumericType val) const
  {
    return m_Value / val;
  }

  NumericType operator /= (NumericType val)
  {
    Set(m_Value / val);
    return m_Value;
  }
 
  NumericType operator % (NumericType val) const
  {
    return m_Value % val;
  }

  NumericType operator %= (NumericType val)
  {
    Set(m_Value % val);
    return m_Value;
  }

  NumericType operator & (NumericType val) const
  {
    return m_Value & val;
  }

  NumericType operator &= (NumericType val)
  {
    Set(m_Value & val);
    return m_Value;
  }

  NumericType operator | (NumericType val) const
  {
    return m_Value | val;
  }

  NumericType operator |= (NumericType val)
  {
    Set(m_Value | val);
    return m_Value;
  }

  NumericType operator ^ (NumericType val) const
  {
    return m_Value ^ val;
  }

  NumericType operator ^= (NumericType val)
  {
    Set(m_Value ^ val);
    return m_Value;
  }

  NumericType operator ~ () const
  {
    return ~m_Value;
  }

  NumericType operator >> (NumericType val) const
  {
    return m_Value >> val;
  }

  NumericType operator >>= (NumericType val)
  {
    Set(m_Value >> val);
    return m_Value;
  }

  NumericType operator << (NumericType val) const
  {
    return m_Value << val;
  }

  NumericType operator <<= (NumericType val)
  {
    Set(m_Value << val);
    return m_Value;
  }

  bool operator == (NumericType val) const
  {
    return m_Value == val;
  }

  bool operator != (NumericType val) const
  {
    return m_Value != val;
  }

  bool operator > (NumericType val) const
  {
    return m_Value > val;
  }

  bool operator >= (NumericType val) const
  {
    return m_Value >= val;
  }

  bool operator < (NumericType val) const
  {
    return m_Value < val;
  }

  bool operator <= (NumericType val) const
  {
    return m_Value <= val;
  }

private:
  void Set(NumericType val)
  {
    m_Value = std::max(Min, std::min(Max, val));
  }

  NumericType m_Value;
};


template <class NumericType, NumericType Min, NumericType Max, class NetBitWriter>
struct NetSerializer<NetRangedNumber<NumericType, Min, Max>, NetBitWriter>
{
  void operator()(const NetRangedNumber<NumericType, Min, Max> & val, NetBitWriter & writer)
  {
    uint64_t ranged_val = (uint64_t)(val - Min);
    writer.WriteBits(ranged_val, GetRequiredBits(Max - Min));
  }
};

template <class NumericType, NumericType Min, NumericType Max, class NetBitReader>
struct NetDeserializer<NetRangedNumber<NumericType, Min, Max>, NetBitReader>
{
  void operator()(NetRangedNumber<NumericType, Min, Max> & val, NetBitReader & reader)
  {
    uint64_t ranged_val = reader.ReadUBits(GetRequiredBits(Max - Min));
    val = static_cast<NumericType>(ranged_val) + Min;
  }
};

