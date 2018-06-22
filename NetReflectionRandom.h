#pragma once

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"
#include "NetRandom.h"

template < class NetBitWriter>
struct NetSerializer<NetRandom, NetBitWriter>
{
  void operator()(const NetRandom & val, NetBitWriter & writer)
  {
    writer.WriteBits(val.GetSeed(), 32);
  }
};

template <class NetBitReader>
struct NetDeserializer<NetRandom, NetBitReader>
{
  void operator()(NetRandom & val, NetBitReader & reader)
  {
    uint32_t seed = (uint32_t)reader.ReadUBits(32);
    val = NetRandom(seed);
  }
};

