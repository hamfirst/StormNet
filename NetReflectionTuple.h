#pragma once

#include <memory>

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetMetaUtil.h"

template <class A, class B, class NetBitWriter>
struct NetSerializer<std::pair<A, B>, NetBitWriter>
{
  void operator()(const std::pair<A, B> & val, NetBitWriter & writer)
  {
    NetSerializer<A, NetBitWriter> serializer1;
    serializer1(val.first, writer);
    NetSerializer<B, NetBitWriter> serializer2;
    serializer2(val.second, writer);
  }
};

template <class A, class B, class NetBitReader>
struct NetDeserializer<std::pair<A, B>, NetBitReader>
{
  void operator()(std::pair<A, B> & val, NetBitReader & reader)
  {
    NetDeserializer<A, NetBitReader> deserializer1;
    deserializer1(val.first, reader);
    NetDeserializer<B, NetBitReader> deserializer2;
    deserializer2(val.second, reader);
  }
};

template <class ... InitArgs, class NetBitWriter>
struct NetSerializer<std::tuple<InitArgs...>, NetBitWriter>
{
  void operator()(const std::tuple<InitArgs...> & val, NetBitWriter & writer)
  {
    auto visitor = [&](auto & f)
    {
      NetSerializer<std::template decay_t<decltype(f)>, NetBitWriter> serializer;
      serializer(f, writer);
    };

    NetMetaUtil::VisitTuple(visitor, val);
  }
};

template <class ... InitArgs, class NetBitReader>
struct NetDeserializer<std::tuple<InitArgs...>, NetBitReader>
{
  void operator()(std::tuple<InitArgs...> & val, NetBitReader & reader)
  {
    auto visitor = [&](auto & f)
    {
      NetDeserializer<std::template decay_t<decltype(f)>, NetBitReader> deserializer;
      deserializer(f, reader);
    };

    NetMetaUtil::VisitTuple(visitor, val);
  }
};

