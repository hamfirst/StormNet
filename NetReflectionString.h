#pragma once

#include <string>

#include "NetSerialize.h"
#include "NetDeserialize.h"

template <class NetBitWriter>
struct NetSerializer<std::string, NetBitWriter>
{
  void operator()(const std::string & val, NetBitWriter & writer)
  {
    auto length = val.length();

    if (val.length() > 127)
    {
      writer.WriteBits(1, 1);
      writer.WriteBits(length, 15);
    }
    else
    {
      writer.WriteBits(0, 1);
      writer.WriteBits(length, 7);
    }

    for (std::size_t index = 0; index < length; index++)
    {
      writer.WriteBits(val[index], 8);
    }
  }
};

template<class NetBitReader>
struct NetDeserializer<std::string, NetBitReader>
{
  void operator()(std::string & val, NetBitReader & reader)
  {
    uint64_t long_string = reader.ReadUBits(1);

    std::size_t length;

    if (long_string)
    {
      length = static_cast<std::size_t>(reader.ReadUBits(15));
    }
    else
    {
      length = static_cast<std::size_t>(reader.ReadUBits(7));
    }

    std::string str;
    str.reserve(length);

    for (std::size_t index = 0; index < length; index++)
    {
      str.push_back(static_cast<char>(reader.ReadSBits(8)));
    }

    val = str;
  }
};


