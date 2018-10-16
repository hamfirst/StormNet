
#pragma once

#include "NetReflectionCommon.h"

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

  ValueType * operator * ()
  {
    return &m_Value;
  }

  const ValueType * operator * () const
  {
    return &m_Value;
  }

private:
  ValueType m_Value;
};

