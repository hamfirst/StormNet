#pragma once

#include "NetDeserialize.h"

template <typename Type, class NetBitReader>
struct NetDeserializerDelta;

template <typename Type, class NetBitReader>
void NetDeserializeValueDelta(Type & val, NetBitReader & reader)
{
  NetDeserializerDelta<Type, NetBitReader> s;
  s(val, reader);
}

template <typename Type, class NetBitReader>
struct NetMemberDeserializeDeltaVisitor
{
  NetMemberDeserializeDeltaVisitor(NetBitReader & reader, std::size_t & next_index, std::size_t & num_remaining) :
    m_Reader(reader), m_NumRemaining(num_remaining), m_NextIndex(next_index), m_RequiredBits(GetRequiredBits(StormReflGetFieldCount<Type>()))
  { }

  template <template <int, typename> class FieldData, int FieldIndex, typename ItrClass>
  static constexpr int ExtractFieldIndex(const FieldData<FieldIndex, ItrClass> & f)
  {
    return FieldIndex;
  }

  template <class FieldData>
  void operator()(FieldData f)
  {
    constexpr int field_index = ExtractFieldIndex(f);
    if (m_NumRemaining == 0)
    {
      return;
    }

    if (field_index != m_NextIndex)
    {
      return;
    }

    NetDeserializeValueDelta(f.Get(), m_Reader);
    m_NumRemaining--;

    if (m_NumRemaining != 0)
    {
      m_NextIndex = static_cast<std::size_t>(m_Reader.ReadUBits(m_RequiredBits));
    }
  }

private:
  NetBitReader & m_Reader;
  std::size_t & m_NumRemaining;
  std::size_t & m_NextIndex;
  int m_RequiredBits;
};

template <class Type, class NetBitReader, std::enable_if_t<StormReflCheckReflectable<Type>::value == false && std::is_standard_layout<Type>::value> * enable = nullptr>
void NetDeserializeDeltaType(Type & val, NetBitReader & reader)
{
  NetDeserializeValue(val, reader);
}

template <class Type, class NetBitReader, std::enable_if_t<StormReflCheckReflectable<Type>::value> * enable = nullptr>
void NetDeserializeDeltaType(Type & val, NetBitReader & reader)
{
  auto required_bits = GetRequiredBits(StormReflGetFieldCount<Type>());
  std::size_t num_elements = static_cast<std::size_t>(reader.ReadUBits(required_bits));

  std::size_t first_index = static_cast<std::size_t>(reader.ReadUBits(required_bits));

  NetMemberDeserializeDeltaVisitor<Type, NetBitReader> deserializer(reader, first_index, num_elements);
  StormReflVisitEach(val, deserializer);
}

template <typename Type, class NetBitReader>
struct NetDeserializerDelta
{
  void operator()(Type & val, NetBitReader & reader)
  {
    NetDeserializeDeltaType(val, reader);
  }
};

template <class NetBitReader>
struct NetDeserializerDelta<std::string, NetBitReader>
{
  void operator()(std::string & val, NetBitReader & reader)
  {
    NetDeserializeValue(val, reader);
  }
};

template <typename Type, int Size, class NetBitReader>
struct NetDeserializerDelta<Type[Size], NetBitReader>
{
  void operator()(Type * val, NetBitReader & reader)
  {
    for (auto index = 0; index < Size; index++)
    {
      NetDeserializeValue(val[index], reader);
    }
  }
};
