
#pragma once

#include "NetReflectionCommon.h"
#include "NetException.h"

template <typename ValueType, typename ParentType, typename Serializer>
class NetReflectionDependent
{
public:
  template <typename ... Args>
  NetReflectionDependent(Args && ... args) :
    m_Value(std::forward<Args>(args)...)
  {

  }

  NetReflectionDependent(const NetReflectionDependent<ValueType, ParentType, Serializer> & rhs) = default;
  NetReflectionDependent(NetReflectionDependent<ValueType, ParentType, Serializer> && rhs) noexcept = default;

  NetReflectionDependent & operator = (const NetReflectionDependent<ValueType, ParentType, Serializer> & rhs) = default;
  NetReflectionDependent & operator = (NetReflectionDependent<ValueType, ParentType, Serializer> && rhs) noexcept = default;

  ValueType * operator -> ()
  {
    return &m_Value;
  }

  const ValueType * operator -> () const
  {
    return &m_Value;
  }

  ValueType & operator * ()
  {
    return m_Value;
  }

  const ValueType & operator * () const
  {
    return m_Value;
  }

private:
  ValueType m_Value;
};


template <typename ValueType, typename ParentType, typename Serializer, typename NetBitWriter>
struct NetSerializer<NetReflectionDependent<ValueType, ParentType, Serializer>, NetBitWriter>
{
  void operator()(const NetReflectionDependent<ValueType, ParentType, Serializer> & val, NetBitWriter & writer)
  {
    auto parent = writer.GetParentObjectAs<ParentType>();
    if (parent == nullptr)
    {
      NET_THROW(std::runtime_error("Could not properly cast parent to depenedent serializer type"));
      return;
    }

    Serializer::Serialize(val, *parent, writer);
  }
};

template <typename ValueType, typename ParentType, typename Serializer, typename NetBitReader>
struct NetDeserializer<NetReflectionDependent<ValueType, ParentType, Serializer>, NetBitReader>
{
  void operator()(NetReflectionDependent<ValueType, ParentType, Serializer> & val, NetBitReader & reader)
  {
    auto parent = reader.GetParentObjectAs<ParentType>();
    if (parent == nullptr)
    {
      NET_THROW(std::runtime_error("Could not properly cast parent to depenedent serializer type"));
      return;
    }

    Serializer::Deserialize(val, *parent, reader);
  }
};


