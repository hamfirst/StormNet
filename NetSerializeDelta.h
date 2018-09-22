#pragma once

#include "NetSerialize.h"

template <typename Type, class NetBitWriter>
struct NetSerializerDelta;

template <typename Type, class NetBitWriter>
bool NetSerializeValueDelta(const Type & to, const Type & from, NetBitWriter & writer)
{
  NetSerializerDelta<Type, NetBitWriter> s;
  return s(to, from, writer);
}

template <typename Type, class NetBitWriter>
struct NetMemberSerializeDeltaVisitor
{
  NetMemberSerializeDeltaVisitor(const Type & other, NetBitWriter & writer, int & num_wrote) :
    m_Other(other), m_Writer(writer), m_NumWrote(num_wrote), m_RequiredBits(GetRequiredBits(StormReflGetFieldCount<Type>()))
  { }

  template <class FieldData>
  void operator()(FieldData f)
  {
    constexpr int field_index = f.GetFieldIndex();

    auto other_f = typename StormReflTypeInfo<Type>::template field_data<field_index, std::add_const_t<Type>>(m_Other);

    auto & target = f.Get();
    auto & source = other_f.Get();

    auto cursor = m_Writer.Reserve(m_RequiredBits);
    if (NetSerializeValueDelta(target, source, m_Writer))
    {
      cursor.WriteBits(field_index, m_RequiredBits);
      m_NumWrote++;
    }
    else
    {
      m_Writer.RollBack(cursor);
    }
  }

private:
  const Type & m_Other;
  NetBitWriter & m_Writer;
  int & m_NumWrote;
  int m_RequiredBits;
};

template <class Type, class NetBitWriter, std::enable_if_t<StormReflCheckReflectable<Type>::value == false && std::is_trivially_copyable<Type>::value> * enable = nullptr>
bool NetSerializeDeltaType(const Type & to, const Type & from, NetBitWriter & writer)
{
  if (to == from)
  {
    return false;
  }

  NetSerializeValue(to, writer);
  return true;
}

template <class Type, class NetBitWriter, std::enable_if_t<StormReflCheckReflectable<Type>::value> * enable = nullptr>
bool NetSerializeDeltaType(const Type & to, const Type & from, NetBitWriter & writer)
{
  int num_wrote = 0;

  auto required_bits = GetRequiredBits(StormReflGetFieldCount<Type>());
  auto cursor = writer.Reserve(required_bits);

  NetMemberSerializeDeltaVisitor<Type, NetBitWriter> serializer(from, writer, num_wrote);
  StormReflVisitEach(to, serializer);

  if (num_wrote > 0)
  {
    cursor.WriteBits(num_wrote, required_bits);
    return true;
  }

  writer.RollBack(cursor);
  return false;
}

template <typename Type, class NetBitWriter>
struct NetSerializerDelta
{
  bool operator()(const Type & to, const Type & from, NetBitWriter & writer)
  {
    return NetSerializeDeltaType(to, from, writer);
  }
};

template <typename Type, class NetBitWriter, int Size>
struct NetSerializerDelta<Type[Size], NetBitWriter>
{
  bool operator()(const Type * to, const Type * from, NetBitWriter & writer)
  {
    auto cursor = writer.Reserve(0);
    int num_differ = 0;

    for (auto index = 0; index < Size; index++)
    {
      if (to[index] != from[index])
      {
        num_differ++;
      }

      NetSerializeValue(to[index], writer);
    }

    if (num_differ == 0)
    {
      writer.RollBack(cursor);
      return false;
    }

    return true;
  }
};

template <class NetBitWriter>
struct NetSerializerDelta<std::string, NetBitWriter>
{
  bool operator()(const std::string & to, const std::string & from, NetBitWriter & writer)
  {
    if (to == from)
    {
      return false;
    }

    NetSerializeValue(to, writer);
    return true;
  }
};
