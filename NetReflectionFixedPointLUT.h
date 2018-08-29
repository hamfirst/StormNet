<<<<<<< HEAD
#pragma once

#include "NetReflectionFixedPoint.h"
#include "NetReflectionFixedPointRange.h"
#include "NetBitUtil.h"

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
struct NetFixedPointVals
{
  using FixedType = NetFixedPoint<StorageType, NumBits, FractionalBits>;

  static const FixedType kZeroF;
  static const FixedType kOneF;
  static const FixedType kTwoF;
  static const FixedType kHalfF;
  static const FixedType kQuarter;
  static const FixedType kEighth;
  static const FixedType kNegOneF;

  static const FixedType kPi;
  static const FixedType kPiOver2;
  static const FixedType kPiOver4;
  static const FixedType kPiOver6;
  static const FixedType kPiOver8;
  static const FixedType kPiTimes2;
  static const FixedType kSqrt2;
  static const FixedType kSqrt3;
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kZeroF = NetFixedPoint<StorageType, NumBits, FractionalBits>(0);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kOneF = NetFixedPoint<StorageType, NumBits, FractionalBits>(1);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kTwoF = NetFixedPoint<StorageType, NumBits, FractionalBits>(2);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kHalfF = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.5");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kQuarter = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.25");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kEighth = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.125");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kNegOneF = NetFixedPoint<StorageType, NumBits, FractionalBits>(-1);

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPi = NetFixedPoint<StorageType, NumBits, FractionalBits>("3.14159265359");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver2 = NetFixedPoint<StorageType, NumBits, FractionalBits>("1.57079632679");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver4 = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.78539816339");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver6 = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.52359877559");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver8 = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.39269908169");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiTimes2 = NetFixedPoint<StorageType, NumBits, FractionalBits>("6.28318530718");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kSqrt2 = NetFixedPoint<StorageType, NumBits, FractionalBits>("1.41421356237");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kSqrt3 = NetFixedPoint<StorageType, NumBits, FractionalBits>("1.73205080757");

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
class NetFixedPointLUT
{
public:
  using FixedType = NetFixedPoint<StorageType, NumBits, FractionalBits>;
  using FixedVals = NetFixedPointVals<StorageType, NumBits, FractionalBits>;

  static const int LUTElements = 2048;
  static const int LUTBits = GetRequiredBits(LUTElements);
  static FixedType kLuAngletInterval;
  static FixedType kLutInterval;


  static_assert(LUTBits <= FractionalBits - 2, "Look up table is bigger than fractional bits");
  static const int kLUTShiftBits = (FractionalBits)-LUTBits + 1;
  static const int kLUTShiftMask = (1 << kLUTShiftBits) - 1;
  static const int kLUTQuarterShiftBits = (FractionalBits - 1) - LUTBits;
  static const int kLUTQuarterShiftMask = (1 << kLUTQuarterShiftBits) - 1;

  static void Init()
  { 
    kLutInterval = FixedType(LUTElements - 1) / FixedVals::kPiOver2;

    int index = 0;
    for(auto val : CreateFixedPointRange(FixedVals::kPiOver2, LUTElements + 1))
    {
      m_SinLut[index] = val.SinSlow();
      m_CosLut[index] = val.CosSlow();
      ++index;
    }

    auto one = FixedType(1);
    index = 0;

    auto threshold = FixedVals::kTwoF;
    threshold -= FixedVals::kSqrt3;

    for (auto val : CreateFixedPointRange(FixedVals::kOneF, LUTElements + 1))
    {
      m_AtanLut[index] = AtanReduced(val, threshold);
      ++index;
    }
  }

  static FixedType Sin(FixedType fixed)
  {
    fixed /= FixedVals::kPiTimes2;
    fixed -= fixed.Floor();

    if (fixed < FixedVals::kHalfF)
    {
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val;
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val;
      }
    }
    else
    {
      fixed -= FixedVals::kHalfF;
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val.Invert();
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val.Invert();
      }
    }
  }

  static FixedType Cos(FixedType fixed)
  {
    fixed /= FixedVals::kPiTimes2;
    fixed -= fixed.Floor();

    if (fixed < FixedVals::kHalfF)
    {
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val;
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val.Invert();
      }
    }
    else
    {
      fixed -= FixedVals::kHalfF;
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val.Invert();
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val;
      }
    }
  }

  static FixedType Atan2(FixedType y, FixedType x)
  {
    if (y.m_Value == 0)
    {
      if (x.m_Value >= 0)
      {
        return FixedType::CreateFromRawVal(0);
      }
      else
      {
        return FixedVals::kPi;
      }
    }

    if (x.m_Value == 0)
    {
      if (y.m_Value >= 0)
      {
        return FixedVals::kPiOver2;
      }
      else
      {
        return FixedVals::kPiOver2 + FixedVals::kPi;
      }
    }

    auto x_abs = x.Abs();
    auto y_abs = y.Abs();

    FixedType val;
    if (x_abs > y_abs)
    {
      auto ratio = y_abs / x_abs;
      val = SampleLut(ratio, kLUTShiftBits, kLUTShiftBits, m_AtanLut);
    }
    else
    {
      auto ratio = x_abs / y_abs;
      auto test_val = ratio.AtanSlow();
      val = SampleLut(ratio, kLUTShiftBits, kLUTShiftBits, m_AtanLut);

      val = FixedVals::kPiOver2 - val;
    }

    if (x.m_Value < 0)
    {
      val = FixedVals::kPi - val;
    }

    if (y.m_Value < 0)
    {
      val = FixedVals::kPiTimes2 - val;
    }

    return val;
  }

private:


  static FixedType SampleLut(FixedType val, int shift_bits, StorageType shift_mask, FixedType * lut)
  {
    auto raw_index = val;
    raw_index.m_Value >>= shift_bits;

    auto index_error = val;
    index_error.m_Value &= shift_mask;
    index_error.m_Value <<= (FractionalBits - shift_bits);

    auto index1 = raw_index.GetRawVal();
    auto index2 = raw_index.GetRawVal() + 1;

    if (index1 == LUTElements)
    {
      return lut[index1];
    }

    auto val1 = lut[index1];
    auto val2 = lut[index2];

    auto diff = (val2 - val1);
    auto delta = index_error * diff;
    auto interpolated_value = val1 + delta;
    return interpolated_value;
  }

  static FixedType AtanReduced(FixedType val, FixedType threshold)
  {
    return val.AtanSlow();
    //if (val <= threshold)
    //{
    //  return val.AtanSlow();
    //}

    //auto new_val = FixedVals::kSqrt3 * val - FixedVals::kOneF;
    //new_val /= (FixedVals::kSqrt3 + val);
    //return FixedVals::kPiOver6 + AtanReduced(new_val, threshold);
  }

  static FixedType m_SinLut[LUTElements + 1];
  static FixedType m_CosLut[LUTElements + 1];
  static FixedType m_AtanLut[LUTElements + 1];
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::kLuAngletInterval = NetFixedPoint<StorageType, NumBits, FractionalBits>(0);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::kLutInterval = NetFixedPoint<StorageType, NumBits, FractionalBits>(0);

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::m_SinLut[NetFixedPointLUT<StorageType, NumBits, FractionalBits>::LUTElements + 1];
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::m_CosLut[NetFixedPointLUT<StorageType, NumBits, FractionalBits>::LUTElements + 1];
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::m_AtanLut[NetFixedPointLUT<StorageType, NumBits, FractionalBits>::LUTElements + 1];


#define INIT_LUT_DATA(StorageType, NumBits, FractionalBits)                                                                                             \
  struct s_reg_class_##StorageType##NumBits##LUTElements                                                                                                \
  {                                                                                                                                                     \
    s_reg_class_##StorageType##NumBits##LUTElements()                                                                                                   \
    {                                                                                                                                                   \
      NetFixedPointLUT<StorageType, NumBits, FractionalBits>::Init();                                                                                   \
    }                                                                                                                                                   \
  } s_reg_class_##StorageType##NumBits##LUTElements;                                                                                                    \


=======
#pragma once

#include "NetReflectionFixedPoint.h"
#include "NetReflectionFixedPointRange.h"
#include "NetBitUtil.h"

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
struct NetFixedPointVals
{
  using FixedType = NetFixedPoint<StorageType, NumBits, FractionalBits>;

  static const FixedType kZeroF;
  static const FixedType kOneF;
  static const FixedType kTwoF;
  static const FixedType kHalfF;
  static const FixedType kQuarter;
  static const FixedType kEighth;
  static const FixedType kNegOneF;

  static const FixedType kPi;
  static const FixedType kPiOver2;
  static const FixedType kPiOver4;
  static const FixedType kPiOver6;
  static const FixedType kPiOver8;
  static const FixedType kPiTimes2;
  static const FixedType kSqrt2;
  static const FixedType kSqrt3;
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kZeroF = NetFixedPoint<StorageType, NumBits, FractionalBits>(0);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kOneF = NetFixedPoint<StorageType, NumBits, FractionalBits>(1);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kTwoF = NetFixedPoint<StorageType, NumBits, FractionalBits>(2);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kHalfF = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.5");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kQuarter = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.25");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kEighth = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.125");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kNegOneF = NetFixedPoint<StorageType, NumBits, FractionalBits>(-1);

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPi = NetFixedPoint<StorageType, NumBits, FractionalBits>("3.14159265359");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver2 = NetFixedPoint<StorageType, NumBits, FractionalBits>("1.57079632679");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver4 = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.78539816339");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver6 = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.52359877559");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiOver8 = NetFixedPoint<StorageType, NumBits, FractionalBits>("0.39269908169");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kPiTimes2 = NetFixedPoint<StorageType, NumBits, FractionalBits>("6.28318530718");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kSqrt2 = NetFixedPoint<StorageType, NumBits, FractionalBits>("1.41421356237");
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
const NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointVals<StorageType, NumBits, FractionalBits>::kSqrt3 = NetFixedPoint<StorageType, NumBits, FractionalBits>("1.73205080757");

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
class NetFixedPointLUT
{
public:
  using FixedType = NetFixedPoint<StorageType, NumBits, FractionalBits>;
  using FixedVals = NetFixedPointVals<StorageType, NumBits, FractionalBits>;

  static const int LUTElements = 2048;
  static const int LUTBits = GetRequiredBits(LUTElements);
  static FixedType kLuAngletInterval;
  static FixedType kLutInterval;


  static_assert(LUTBits <= FractionalBits - 2, "Look up table is bigger than fractional bits");
  static const int kLUTShiftBits = (FractionalBits)-LUTBits + 1;
  static const int kLUTShiftMask = (1 << kLUTShiftBits) - 1;
  static const int kLUTQuarterShiftBits = (FractionalBits - 1) - LUTBits;
  static const int kLUTQuarterShiftMask = (1 << kLUTQuarterShiftBits) - 1;

  static void Init()
  { 
    kLutInterval = FixedType(LUTElements - 1) / FixedVals::kPiOver2;

    int index = 0;
    for(auto val : CreateFixedPointRange(FixedVals::kPiOver2, LUTElements + 1))
    {
      m_SinLut[index] = val.SinSlow();
      m_CosLut[index] = val.CosSlow();
      ++index;
    }

    auto one = FixedType(1);
    index = 0;

    auto threshold = FixedVals::kTwoF;
    threshold -= FixedVals::kSqrt3;

    for (auto val : CreateFixedPointRange(FixedVals::kOneF, LUTElements + 1))
    {
      m_AtanLut[index] = AtanReduced(val, threshold);
      ++index;
    }
  }

  static FixedType Sin(FixedType fixed)
  {
    fixed /= FixedVals::kPiTimes2;
    fixed -= fixed.Floor();

    if (fixed < FixedVals::kHalfF)
    {
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val;
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val;
      }
    }
    else
    {
      fixed -= FixedVals::kHalfF;
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val.Invert();
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_SinLut);
        return val.Invert();
      }
    }
  }

  static FixedType Cos(FixedType fixed)
  {
    fixed /= FixedVals::kPiTimes2;
    fixed -= fixed.Floor();

    if (fixed < FixedVals::kHalfF)
    {
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val;
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val.Invert();
      }
    }
    else
    {
      fixed -= FixedVals::kHalfF;
      if (fixed < FixedVals::kQuarter)
      {
        auto val = SampleLut(fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val.Invert();
      }
      else
      {
        auto val = SampleLut(FixedVals::kHalfF - fixed, kLUTQuarterShiftBits, kLUTQuarterShiftBits, m_CosLut);
        return val;
      }
    }
  }

  static FixedType Atan2(FixedType y, FixedType x)
  {
    if (y.m_Value == 0)
    {
      if (x.m_Value >= 0)
      {
        return FixedType::CreateFromRawVal(0);
      }
      else
      {
        return FixedVals::kPi;
      }
    }

    if (x.m_Value == 0)
    {
      if (y.m_Value >= 0)
      {
        return FixedVals::kPiOver2;
      }
      else
      {
        return FixedVals::kPiOver2 + FixedVals::kPi;
      }
    }

    auto x_abs = x.Abs();
    auto y_abs = y.Abs();

    FixedType val;
    if (x_abs > y_abs)
    {
      auto ratio = y_abs / x_abs;
      val = SampleLut(ratio, kLUTShiftBits, kLUTShiftBits, m_AtanLut);
    }
    else
    {
      auto ratio = x_abs / y_abs;
      auto test_val = ratio.AtanSlow();
      val = SampleLut(ratio, kLUTShiftBits, kLUTShiftBits, m_AtanLut);

      val = FixedVals::kPiOver2 - val;
    }

    if (x.m_Value < 0)
    {
      val = FixedVals::kPi - val;
    }

    if (y.m_Value < 0)
    {
      val = FixedVals::kPiTimes2 - val;
    }

    return val;
  }

private:


  static FixedType SampleLut(FixedType val, int shift_bits, StorageType shift_mask, FixedType * lut)
  {
    auto raw_index = val;
    raw_index.m_Value >>= shift_bits;

    auto index_error = val;
    index_error.m_Value &= shift_mask;
    index_error.m_Value <<= (FractionalBits - shift_bits);

    auto index1 = raw_index.GetRawVal();
    auto index2 = raw_index.GetRawVal() + 1;

    if (index1 == LUTElements)
    {
      return lut[index1];
    }

    auto val1 = lut[index1];
    auto val2 = lut[index2];

    auto diff = (val2 - val1);
    auto delta = index_error * diff;
    auto interpolated_value = val1 + delta;
    return interpolated_value;
  }

  static FixedType AtanReduced(FixedType val, FixedType threshold)
  {
    return val.AtanSlow();
    //if (val <= threshold)
    //{
    //  return val.AtanSlow();
    //}

    //auto new_val = FixedVals::kSqrt3 * val - FixedVals::kOneF;
    //new_val /= (FixedVals::kSqrt3 + val);
    //return FixedVals::kPiOver6 + AtanReduced(new_val, threshold);
  }

  static FixedType m_SinLut[LUTElements + 1];
  static FixedType m_CosLut[LUTElements + 1];
  static FixedType m_AtanLut[LUTElements + 1];
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::kLuAngletInterval = NetFixedPoint<StorageType, NumBits, FractionalBits>(0);
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::kLutInterval = NetFixedPoint<StorageType, NumBits, FractionalBits>(0);

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::m_SinLut[NetFixedPointLUT<StorageType, NumBits, FractionalBits>::LUTElements + 1];
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::m_CosLut[NetFixedPointLUT<StorageType, NumBits, FractionalBits>::LUTElements + 1];
template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPoint<StorageType, NumBits, FractionalBits> NetFixedPointLUT<StorageType, NumBits, FractionalBits>::m_AtanLut[NetFixedPointLUT<StorageType, NumBits, FractionalBits>::LUTElements + 1];


#define INIT_LUT_DATA(StorageType, NumBits, FractionalBits)                                                                                             \
  struct s_reg_class_##StorageType##NumBits##LUTElements                                                                                                \
  {                                                                                                                                                     \
    s_reg_class_##StorageType##NumBits##LUTElements()                                                                                                   \
    {                                                                                                                                                   \
      NetFixedPointLUT<StorageType, NumBits, FractionalBits>::Init();                                                                                   \
    }                                                                                                                                                   \
  } s_reg_class_##StorageType##NumBits##LUTElements;                                                                                                    \


>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
