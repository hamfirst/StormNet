
#pragma once


#include <StormRefl/StormReflMetaEnum.h>

#include <hash/Hash.h>

#include "NetReflectionCommon.h"

template <class EnumType>
class NetEnum
{
public:

  NetEnum() :
    m_Value((EnumType)0)
  {

  }

  NetEnum(const NetEnum<EnumType> & rhs) :
    m_Value(rhs.m_Value)
  {
  }

  NetEnum(NetEnum<EnumType> && rhs) :
    m_Value(rhs.m_Value)
  {

  }

  NetEnum(EnumType val) :
    m_Value(val)
  {
  }

  const EnumType & operator = (EnumType val)
  {
    Set(val);
    return m_Value;
  }

  const EnumType & operator = (const char * val)
  {
    auto hash = crc32(val);
    EnumType out;

    if (StormReflGetEnumFromHash(out, hash))
    {
      Set(out);
    }

    return m_Value;
  }

  const EnumType & operator = (const NetEnum<EnumType> & rhs)
  {
    Set(rhs.m_Value);
    return m_Value;
  }

  const EnumType & operator = (NetEnum<EnumType> && rhs)
  {
    Set(rhs.m_Value);
    return m_Value;
  }

  operator int() const
  {
    return (int)m_Value;
  }

  operator EnumType() const
  {
    return m_Value;
  }

  czstr ToString() const
  {
    return StormReflGetEnumAsString(m_Value);
  }

  EnumType ToEnum() const
  {
    return m_Value;
  }

  bool operator == (EnumType val) const
  {
    return m_Value == val;
  }

  bool operator == (const NetEnum<EnumType> & val) const
  {
    return m_Value == val.m_Value;
  }

  bool operator != (EnumType val) const
  {
    return m_Value != val;
  }

  bool operator < (EnumType val) const
  {
    return m_Value < val;
  }

  bool operator <= (EnumType val) const
  {
    return m_Value <= val;
  }

  bool operator > (EnumType val) const
  {
    return m_Value > val;
  }

  bool operator >= (EnumType val) const
  {
    return m_Value >= val;
  }

private:
  void Set(EnumType val)
  {
    m_Value = val;
  }

  EnumType m_Value;
};

template <typename EnumType, class NetBitWriter>
struct NetSerializer<NetEnum<EnumType>, NetBitWriter>
{
  void operator()(const NetEnum<EnumType> & val, NetBitWriter & writer)
  {
    writer.WriteBits(static_cast<int>(val), GetRequiredBits(StormReflGetEnumElemCount<EnumType>()));
  }
};

template <typename EnumType, class NetBitReader>
struct NetDeserializer<NetEnum<EnumType>, NetBitReader>
{
  void operator()(NetEnum<EnumType> & val, NetBitReader & reader)
  {
    uint64_t enum_val = reader.ReadUBits(GetRequiredBits(StormReflGetEnumElemCount<EnumType>()));
    val = static_cast<EnumType>(enum_val);
  }
};
