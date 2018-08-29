<<<<<<< HEAD
#pragma once

#include <memory>

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"

template <typename DataType, bool DefaultConstructable>
struct NetPtrDefaultConstruct
{
  static std::unique_ptr<DataType> ConstructUnique()
  {
    throw false;
  }

  static std::shared_ptr<DataType> ConstructShared()
  {
    throw false;
  }
};

template <typename DataType>
struct NetPtrDefaultConstruct<DataType, true>
{
  static std::unique_ptr<DataType> ConstructUnique()
  {
    return std::make_unique<DataType>();
  }

  static std::shared_ptr<DataType> ConstructShared()
  {
    return std::make_shared<DataType>();
  }
};

template <class T, class NetBitWriter>
struct NetSerializer<std::unique_ptr<T>, NetBitWriter>
{
  void operator()(const std::unique_ptr<T> & val, NetBitWriter & writer)
  {
    if (val)
    {
      writer.WriteBits(1, 1);

      NetSerializer<T, NetBitWriter> serializer;
      serializer(*val.get(), writer);
    }
    else
    {
      writer.WriteBits(0, 1);
    }
  }
};

template <class T, class NetBitReader>
struct NetDeserializer<std::unique_ptr<T>, NetBitReader>
{
  void operator()(std::unique_ptr<T> & val, NetBitReader & reader)
  {
    if (reader.ReadUBits(1))
    {
      if (val.get() == nullptr)
      {
        val = NetPtrDefaultConstruct<T, std::is_default_constructible<T>::value>::ConstructUnique();
      }

      NetDeserializer<T, NetBitReader> deserializer;
      deserializer(*val.get(), reader);
    }
    else
    {
      val.reset();
    }
  }
};

template <class T, class NetBitWriter>
struct NetSerializer<std::shared_ptr<T>, NetBitWriter>
{
  void operator()(const std::shared_ptr<T> & val, NetBitWriter & writer)
  {
    if (val)
    {
      writer.WriteBits(1, 1);

      NetSerializer<T, NetBitWriter> serializer;
      serializer(*val.get(), writer);
    }
    else
    {
      writer.WriteBits(0, 1);
    }
  }
};

template <class T, class NetBitReader>
struct NetDeserializer<std::shared_ptr<T>, NetBitReader>
{
  void operator()(std::shared_ptr<T> & val, NetBitReader & reader)
  {
    if (reader.ReadUBits(1))
    {
      if (val.get() == nullptr)
      {
        val = NetPtrDefaultConstruct<T, std::is_default_constructible<T>::value>::ConstructShared();
      }

      NetDeserializer<T, NetBitReader> deserializer;
      deserializer(*val.get(), reader);
    }
    else
    {
      val.reset();
    }
  }
};

=======
#pragma once

#include <memory>

#include "NetReflectionCommon.h"

#include "NetSerialize.h"
#include "NetDeserialize.h"

template <typename DataType, bool DefaultConstructable>
struct NetPtrDefaultConstruct
{
  static std::unique_ptr<DataType> ConstructUnique()
  {
    throw false;
  }

  static std::shared_ptr<DataType> ConstructShared()
  {
    throw false;
  }
};

template <typename DataType>
struct NetPtrDefaultConstruct<DataType, true>
{
  static std::unique_ptr<DataType> ConstructUnique()
  {
    return std::make_unique<DataType>();
  }

  static std::shared_ptr<DataType> ConstructShared()
  {
    return std::make_shared<DataType>();
  }
};

template <class T, class NetBitWriter>
struct NetSerializer<std::unique_ptr<T>, NetBitWriter>
{
  void operator()(const std::unique_ptr<T> & val, NetBitWriter & writer)
  {
    if (val)
    {
      writer.WriteBits(1, 1);

      NetSerializer<T, NetBitWriter> serializer;
      serializer(*val.get(), writer);
    }
    else
    {
      writer.WriteBits(0, 1);
    }
  }
};

template <class T, class NetBitReader>
struct NetDeserializer<std::unique_ptr<T>, NetBitReader>
{
  void operator()(std::unique_ptr<T> & val, NetBitReader & reader)
  {
    if (reader.ReadUBits(1))
    {
      if (val.get() == nullptr)
      {
        val = NetPtrDefaultConstruct<T, std::is_default_constructible<T>::value>::ConstructUnique();
      }

      NetDeserializer<T, NetBitReader> deserializer;
      deserializer(*val.get(), reader);
    }
    else
    {
      val.reset();
    }
  }
};

template <class T, class NetBitWriter>
struct NetSerializer<std::shared_ptr<T>, NetBitWriter>
{
  void operator()(const std::shared_ptr<T> & val, NetBitWriter & writer)
  {
    if (val)
    {
      writer.WriteBits(1, 1);

      NetSerializer<T, NetBitWriter> serializer;
      serializer(*val.get(), writer);
    }
    else
    {
      writer.WriteBits(0, 1);
    }
  }
};

template <class T, class NetBitReader>
struct NetDeserializer<std::shared_ptr<T>, NetBitReader>
{
  void operator()(std::shared_ptr<T> & val, NetBitReader & reader)
  {
    if (reader.ReadUBits(1))
    {
      if (val.get() == nullptr)
      {
        val = NetPtrDefaultConstruct<T, std::is_default_constructible<T>::value>::ConstructShared();
      }

      NetDeserializer<T, NetBitReader> deserializer;
      deserializer(*val.get(), reader);
    }
    else
    {
      val.reset();
    }
  }
};

>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
