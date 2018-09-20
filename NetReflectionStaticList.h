#pragma once

#include <vector>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

template <typename DataType, const DataType * DataList, const int * DataSize>
class NetReflectionStaticListPtr
{
public:
    NetReflectionStaticListPtr() = default;
    NetReflectionStaticListPtr(const NetReflectionStaticListPtr<DataType, DataList, DataSize> & rhs) = default;
    NetReflectionStaticListPtr & operator = (const NetReflectionStaticListPtr<DataType, DataList, DataSize> & rhs) = default;
    
    NetReflectionStaticListPtr & operator = (const DataType & val)
    {
        return operator = (&val);
    }

    NetReflectionStaticListPtr & operator = (const DataType * val)
    {
        for(int index = 0; index < *DataSize; ++index)
        {
            auto elem = &DataList[index];
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
        return DataList[m_CurrentIndex];
    }

    const DataType * operator -> () const
    {
        return &DataList[m_CurrentIndex];
    }

    operator const DataType * () const
    {
        return &DataList[m_CurrentIndex];
    }

    const DataType & Value() const
    {
        return DataList[m_CurrentIndex];
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

template <typename DataType, const DataType * DataList, const int * DataSize, class NetBitWriter>
struct NetSerializer<NetReflectionStaticListPtr<DataType, DataList, DataSize>, NetBitWriter>
{
  void operator()(const NetReflectionStaticListPtr<DataType, DataList, DataSize> & val, NetBitWriter & writer)
  {
      auto required_bits = GetRequiredBits(*DataSize);
      writer.WriteBits(val.CurrentIndex(), required_bits);
  }
};

template <typename DataType, const DataType * DataList, const int * DataSize, class NetBitReader>
struct NetDeserializer<NetReflectionStaticListPtr<DataType, DataList, DataSize>, NetBitReader>
{
  void operator()(NetReflectionStaticListPtr<DataType, DataList, DataSize> & val, NetBitReader & reader)
  {
      auto required_bits = GetRequiredBits(*DataSize);
      val = DataList[reader.ReadUBits(required_bits)];
  }
};
