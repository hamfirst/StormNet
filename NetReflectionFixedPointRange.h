#pragma once

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
class NetFixedPoint;

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
struct NetFixedPointRangeInclusive
{
  NetFixedPointRangeInclusive(StorageType start, StorageType end, int num_segs)
  {
    auto offset = end - start;

    m_Val = start;
    m_Inc = offset / (num_segs - 1);
    m_Rem = offset % (num_segs - 1);
    m_HalfRem = m_Rem / 2;
    m_Final = end;
    m_Elems = num_segs - 1;
  }

  struct Iterator
  {
    StorageType m_Val;
    StorageType m_FinalVal;
    StorageType m_Inc;
    StorageType m_Rem;
    StorageType m_HalfRem;
    StorageType m_Extra;
    StorageType m_Final;
    StorageType m_Elems;

    Iterator & operator ++()
    {
      m_Val += m_Inc;
      m_Extra += m_Rem;

      m_FinalVal = m_Val;
      if (m_Extra + m_HalfRem > m_Rem)
      {
        m_FinalVal++;
      }

      if (m_Extra > m_Rem)
      {
        m_Val += 1;
        m_Extra -= m_Rem;
      }

      m_Elems--;
      return *this;
    }

    NetFixedPoint<StorageType, NumBits, FractionalBits> operator * () const
    {
      return NetFixedPoint<StorageType, NumBits, FractionalBits>::CreateFromRawVal(m_Elems == 0 ? m_Final : m_FinalVal);
    }

    bool operator == (const Iterator & rhs) const
    {
      return m_Elems == rhs.m_Elems;
    }

    bool operator != (const Iterator & rhs) const
    {
      return m_Elems != rhs.m_Elems;
    }
  };

  Iterator begin()
  {
    Iterator itr;
    itr.m_Val = m_Val;
    itr.m_FinalVal = m_Val;
    itr.m_Inc = m_Inc;
    itr.m_Rem = m_Rem;
    itr.m_HalfRem = m_HalfRem;
    itr.m_Extra = 0;
    itr.m_Elems = m_Elems;
    itr.m_Final = m_Final;
    return itr;
  }

  Iterator end()
  {
    Iterator itr;
    itr.m_Val = m_Final;
    itr.m_FinalVal = m_Final;
    itr.m_Inc = m_Inc;
    itr.m_Rem = m_Rem;
    itr.m_HalfRem = m_HalfRem;
    itr.m_Extra = 0;
    itr.m_Elems = -1;
    itr.m_Final = m_Final;
    return itr;
  }
  
  StorageType m_Val;
  StorageType m_Inc;
  StorageType m_Rem;
  StorageType m_HalfRem;
  StorageType m_Final;
  StorageType m_Elems;
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPointRangeInclusive<StorageType, NumBits, FractionalBits> CreateFixedPointRange(
  const NetFixedPoint<StorageType, NumBits, FractionalBits> & start, 
  const NetFixedPoint<StorageType, NumBits, FractionalBits> & end, int num_segs)
{
  return NetFixedPointRangeInclusive<StorageType, NumBits, FractionalBits>(start.GetRawVal(), end.GetRawVal(), num_segs);
};

template <typename StorageType, StorageType NumBits, StorageType FractionalBits>
NetFixedPointRangeInclusive<StorageType, NumBits, FractionalBits> CreateFixedPointRange(
  const NetFixedPoint<StorageType, NumBits, FractionalBits> & end, int num_segs)
{
  return NetFixedPointRangeInclusive<StorageType, NumBits, FractionalBits>(0, end.GetRawVal(), num_segs);
};
