#pragma once

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetException.h"

#include <xmmintrin.h>

#pragma warning(push)
#pragma warning(disable : 4293)

inline int64_t FixedPointMultiplyWide(int64_t a, int64_t b, int FractionalBits)
{
#if !defined(WIN32) && !defined(_LINUX) && !defined(STORM_REFL_PARSE)
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
  NET_THROW_OR("not implemented :(", return 0);
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

#ifndef WIN32
inline int64_t FixedPointDivideWide(int64_t a, int64_t b, int FractionalBits)
{
  NET_THROW_OR("not implemented :(", return 0);
}
#endif

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

  NetFixedPoint() = default;
  NetFixedPoint(const FixedType & rhs) = default;
  NetFixedPoint(FixedType && rhs) = default;

  FixedType & operator = (const FixedType & rhs) = default;
  FixedType & operator = (FixedType && rhs) = default;

  explicit NetFixedPoint(int8_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  explicit NetFixedPoint(int16_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  explicit NetFixedPoint(int32_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  explicit NetFixedPoint(int64_t val)
  {
    m_Value = (((StorageType)val) << FractionalBits);
  }

  explicit NetFixedPoint(float val)
  {
    m_Value = ((StorageType)(val * kOne));
  }

  explicit NetFixedPoint(double val)
  {
    m_Value = ((StorageType)(val * kOne));
  }

  static FixedType CreateFromRawVal(StorageType val)
  {
    FixedType f;
    f.m_Value = val;
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
    return m_Value >> FractionalBits;
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
    return CreateFromRawVal(m_Value + val);
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
    Set(m_Value - val);
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

  FixedType Frac()
  {
    return CreateFromRawVal(m_Value & kFractionalMask);
  }

  FixedType Floor()
  {
    return CreateFromRawVal(m_Value & kDecimalMask);
  }

  FixedType Ceil()
  {
    if ((m_Value & kFractionalMask) == 0)
    {
      return Floor();
    }

    return CreateFromRawVal(m_Value + kOne);
  }

private:

  void Set(StorageType val)
  {
    m_Value = (val & kMask);
  }

  static const StorageType kMask = NumBits >= sizeof(StorageType) ? StorageType(-1) : ((StorageType(1) << NumBits) - 1);
  static const StorageType kFractionalMask = (StorageType(1) << FractionalBits) - 1;
  static const StorageType kDecimalMask = (~kFractionalMask) & kMask;

  static const StorageType kOne = StorageType(1) << FractionalBits;


  StorageType m_Value;
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
struct NetFixedPointVals
{
  using FixedType = NetFixedPoint<StorageType, NumBits, FractionalBits>;
  static const FixedType kOneF = FixedType(1.0f);
  static const FixedType kTwoF = FixedType(2.0f);
  static const FixedType kHalfF = FixedType(0.5f);
  static const FixedType kNegOneF = FixedType(-1.0f);

  static const FixedType kPi = FixedType(3.14159265359);
  static const FixedType kPiOver2 = FixedType(1.57079632679);
  static const FixedType kPiTime2 = FixedType(6.28318530718);

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