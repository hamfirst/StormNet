#pragma once

#include <vector>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"



template <typename DataType, DataType * const * DataList, const int * DataSize, bool DelegateDereference = false>
class NetReflectionStaticListPtr
{
public:
    NetReflectionStaticListPtr() = default;
    NetReflectionStaticListPtr(const NetReflectionStaticListPtr<DataType, DataList, DataSize, DelegateDereference> & rhs) = default;
    NetReflectionStaticListPtr & operator = (const NetReflectionStaticListPtr<DataType, DataList, DataSize, DelegateDereference> & rhs) = default;
    
    NetReflectionStaticListPtr & operator = (const DataType & val)
    {
        return operator = (&val);
    }

    NetReflectionStaticListPtr & operator = (const DataType * val)
    {
        for(int index = 0; index < *DataSize; ++index)
        {
            auto elem = &(*DataList)[index];
            if(val == elem)
            {
                m_CurrentIndex = index;
                return *this;
            }
        }

        NET_THROW(std::out_of_range("Element does not exist in the list"));
        return *this;
    }

    const DataType & operator * () const
    {
        return (*DataList)[m_CurrentIndex];
    }

    template <typename ValType>
    void SetTo(ValType && val)
    {
      for(int index = 0; index < *DataSize; ++index)
      {
        auto & elem = (*DataList)[index];
        if(elem == val)
        {
          m_CurrentIndex = index;
          return;
        }
      }

      NET_THROW(std::out_of_range("Element does not exist in the list"));
    }

    decltype(auto) operator -> () const
    {
        if constexpr(DelegateDereference)
        {
            return (*DataList)[m_CurrentIndex];
        }
        else
        {
            return &(*DataList)[m_CurrentIndex];
        }
    }

    operator const DataType * () const
    {
        return &(*DataList)[m_CurrentIndex];
    }

    const DataType & Value() const
    {
        return (*DataList)[m_CurrentIndex];
    }

    int CurrentIndex() const
    {
        return m_CurrentIndex;
    }

    bool operator == (const NetReflectionStaticListPtr<DataType, DataList, DataSize> & rhs) const
    {
        return m_CurrentIndex == rhs.m_CurrentIndex;
    }

private:
    int m_CurrentIndex = 0;
};

template <typename DataType, DataType * const * DataList, const int * DataSize, bool DelegateDereference, class NetBitWriter>
struct NetSerializer<NetReflectionStaticListPtr<DataType, DataList, DataSize, DelegateDereference>, NetBitWriter>
{
  void operator()(const NetReflectionStaticListPtr<DataType, DataList, DataSize, DelegateDereference> & val, NetBitWriter & writer)
  {
      auto required_bits = GetRequiredBits(*DataSize);
      writer.WriteBits(val.CurrentIndex(), required_bits);
  }
};

template <typename DataType, DataType * const * DataList, const int * DataSize, bool DelegateDereference, class NetBitReader>
struct NetDeserializer<NetReflectionStaticListPtr<DataType, DataList, DataSize, DelegateDereference>, NetBitReader>
{
  void operator()(NetReflectionStaticListPtr<DataType, DataList, DataSize, DelegateDereference> & val, NetBitReader & reader)
  {
      auto required_bits = GetRequiredBits(*DataSize);
      val = DataList[reader.ReadUBits(required_bits)];
  }
};
