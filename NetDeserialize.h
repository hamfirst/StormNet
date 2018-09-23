#pragma once

#include <StormRefl/StormReflMetaFuncs.h>

#include "NetBitReader.h"
#include "NetBitWriter.h"
#include "NetBitUtil.h"

#include "NetReflectionCommon.h"



template <class Type, class NetBitReader>
void NetDeserializeValue(Type & t, NetBitReader && reader)
{
  NetDeserializer<Type, NetBitReader> s;
  s(t, reader);
}

template <class Type, class NetBitReader, std::enable_if_t<StormReflCheckReflectable<Type>::value == false && std::is_trivially_copyable<Type>::value> * enable = nullptr>
void NetDeserializeType(Type & val, NetBitReader && reader)
{
  char * mem_ptr = (char *)&val;
  char * end_ptr = mem_ptr + sizeof(Type);

  while (mem_ptr != end_ptr)
  {
    *mem_ptr = static_cast<char>(reader.ReadUBits(8));
    mem_ptr++;
  }
}

template <typename Type, class NetBitReader, typename std::enable_if<StormReflCheckReflectable<Type>::value>::type * = nullptr>
void NetDeserializeType(Type & val, NetBitReader && reader)
{
  auto deserialize_visitor = [&](auto f) 
  {
    using FieldMetaInfo = typename std::template decay_t<decltype(f)>;
    if (StormReflHasAnnotation<Type, FieldMetaInfo::GetFieldIndex()>("static"))
    {
      return;
    }

    NetDeserializeValue(f.Get(), reader); 
  };
  StormReflVisitEach(val, deserialize_visitor);
};

template <typename Type, class NetBitReader>
struct NetDeserializer
{
  void operator()(Type & type, NetBitReader & reader)
  {
    NetDeserializeType(type, reader);
  }
};

template<class NetBitReader>
struct NetDeserializer<bool, NetBitReader>
{
  void operator()(bool & val, NetBitReader & reader)
  {
    val = reader.ReadUBits(1) != 0;
  }
};
