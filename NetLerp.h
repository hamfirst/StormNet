#pragma once

#include <StormRefl/StormReflMetaFuncs.h>

#include "NetReflectionList.h"
#include "NetReflectionMap.h"
#include "NetReflectionNumber.h"
#include "NetReflectionFixedPoint.h"

template <typename T>
void NetLerp(const T & a, const T & b, T & dst, float fac);

template <typename T>
struct NetLerpStruct
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    auto visitor = [&](auto fa, auto fb, auto fdest) 
    {
      NetLerp(fa.Get(), fb.Get(), fdest.Get(), fac);
    };

    StormReflVisitEach(a, b, dst, visitor);
  }
};

template <typename T>
struct NetLerpValNumeric
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    //dst = (T)((a * (1.0f - fac)) + (b * fac));
    dst = b;
  }
};

template <typename T>
struct NetLerpValDefault
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    dst = b;
  }
};

template <typename T, std::size_t Size>
struct NetLerpValDefault<T[Size]>
{
  static void Process(const T * a, const T * b, T * dst, float fac)
  {
    for (std::size_t index = 0; index < Size; index++)
    {
      NetLerp(a[index], b[index], dst[index], fac);
    }
  }
};

template <typename T, bool IsRefl>
struct NetLerpCheckClass
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    NetLerpValDefault<T>::Process(a, b, dst, fac);
  }
};

template <typename T>
struct NetLerpCheckClass<T, true>
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    NetLerpStruct<T>::Process(a, b, dst, fac);
  }
};

template <typename T, bool IsNumeric>
struct NetLerpCheckNumeric
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    NetLerpCheckClass<T, std::is_class<T>::value && StormReflCheckReflectable<T>::value>::Process(a, b, dst, fac);
  }
};

template <typename T>
struct NetLerpCheckNumeric<T, true>
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    NetLerpValNumeric<T>::Process(a, b, dst, fac);
  }
};

template <typename T>
struct NetLerpVal
{
  static void Process(const T & a, const T & b, T & dst, float fac)
  {
    NetLerpCheckNumeric<T, std::is_floating_point<T>::value>::Process(a, b, dst, fac);
  }
};

template <typename T, std::size_t MaxSize>
struct NetLerpVal<NetSparseList<T, MaxSize>>
{
  static void Process(const NetSparseList<T, MaxSize> & a, const NetSparseList<T, MaxSize> & b, NetSparseList<T, MaxSize> & dst, float fac)
  {
    dst.Clear();
    for (auto elem : b)
    {
      if (a.HasAt(elem.first))
      {
        NetLerp(a[elem.first], b[elem.first], dst.EmplaceAt(elem.first, elem.second), fac);
      }
      else
      {
        dst.EmplaceAt(elem.first, elem.second);
      }
    }
  }
};

template <typename T, std::size_t MaxSize>
struct NetLerpVal<NetArrayList<T, MaxSize>>
{
  static void Process(const NetArrayList<T, MaxSize> & a, const NetArrayList<T, MaxSize> & b, NetArrayList<T, MaxSize> & dst, float fac)
  {
    dst.clear();
    for(std::size_t index = 0; index < b.size(); index++)
    {
      if (index < a.size())
      {
        NetLerp(a[index], b[index], dst.EmplaceBack(), fac);
      }
      else
      {
        dst.EmplaceBack(b[index]);
      }
    }
  }
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
struct NetLerpVal<NetFixedPoint<StorageType, NumBits, FractionalBits>>
{
  static void Process(const NetFixedPoint<StorageType, NumBits, FractionalBits> & a, const NetFixedPoint<StorageType, NumBits, FractionalBits> & b, NetFixedPoint<StorageType, NumBits, FractionalBits> & dst, float fac)
  {
    dst = (StorageType)(((float)a * (1.0f - fac)) + ((float)b * fac));
  }
};

template <typename T>
void NetLerp(const T & a, const T & b, T & dst, float fac)
{
  NetLerpVal<T>::Process(a, b, dst, fac);
}
