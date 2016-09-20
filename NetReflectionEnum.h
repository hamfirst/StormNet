#pragma once

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"


template <class EnumType>
class NetEnum
{
public:

  NetEnum()
  {
    
  }

  NetEnum(EnumType val)
  {
    m_Value = val;
  }
  
  const EnumType & operator = (EnumType val)
  {
    Set(val);
    return m_Value;
  }

  operator int() const
  {
    return m_Value._to_integral();
  }

  operator EnumType() const
  {
    return m_Value;
  }

  int _to_integral() const
  {
    return m_Value._to_integral();
  }

  czstr _to_string() const
  {
    return m_Value._to_string();
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

#ifdef REFLECTION_CHANGE_NOTIFIER
    ReflectionNotifySet(m_ReflectionInfo, _to_string());
#endif
  }

  EnumType m_Value;
};

template <typename EnumType, class NetBitWriter>
struct NetSerializer<NetEnum<EnumType>, NetBitWriter>
{
  void operator()(const NetEnum<EnumType> & val, NetBitWriter & writer)
  {
    writer.WriteBits(static_cast<int>(val), GetRequiredBits(EnumType::_size()));
  }
};

template <typename EnumType, class NetBitReader>
struct NetDeserializer<NetEnum<EnumType>, NetBitReader>
{
  void operator()(NetEnum<EnumType> & val, NetBitReader & reader)
  {
    uint64_t enum_val = reader.ReadUBits(GetRequiredBits(EnumType::_size()));
    val = static_cast<EnumType>(enum_val);
  }
};
