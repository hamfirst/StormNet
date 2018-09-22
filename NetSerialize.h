#pragma once

#include <StormRefl/StormReflMetaFuncs.h>

#include "NetBitReader.h"
#include "NetBitWriter.h"
#include "NetBitUtil.h"

#include "NetReflectionCommon.h"

template <typename Type, class NetBitWriter>
struct NetSerializer;

template <class Type, class NetBitWriter>
void NetSerializeValue(const Type & t, NetBitWriter & writer)
{
  NetSerializer<Type, NetBitWriter> s;
  s(t, writer);
}

template <class Type, class NetBitWriter, std::enable_if_t<StormReflCheckReflectable<Type>::value == false && std::is_trivially_copyable<Type>::value> * enable = nullptr>
void NetSerializeType(const Type & val, NetBitWriter & writer)
{
  const char * mem_ptr = (const char *)&val;
  const char * end_ptr = mem_ptr + sizeof(Type);

  while (mem_ptr != end_ptr)
  {
    writer.WriteBits((uint64_t)(*mem_ptr & 0xFF), 8);
    mem_ptr++;
  }
}

template <typename Type, class NetBitWriter, typename std::enable_if<StormReflCheckReflectable<Type>::value>::type * = nullptr>
void NetSerializeType(const Type & val, NetBitWriter & writer)
{
  auto serialize_visitor = [&](auto f) 
  {
    using FieldMetaInfo = typename std::template decay_t<decltype(f)>;
    if (StormReflHasAnnotation<Type, FieldMetaInfo::GetFieldIndex()>("static"))
    {
      return;
    }

    NetSerializeValue(f.Get(), writer); 
  };
  StormReflVisitEach(val, serialize_visitor);
};

template <typename Type, class NetBitWriter>
struct NetSerializer
{
  void operator()(const Type & type, NetBitWriter & writer)
  {
    NetSerializeType(type, writer);
  }
};

template <class NetBitWriter>
struct NetSerializer<bool, NetBitWriter>
{
  void operator()(const bool & val, NetBitWriter & writer)
  {
    writer.WriteBits(val, 1);
  }
};

