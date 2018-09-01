#pragma once

#include <vector>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

template <typename DataType, const std::vector<DataType> * DataList>
class NetReflectionStaticListPtr
{
public:
    NetReflectionStaticListPtr();

    NetReflectionStaticListPtr(const NetReflectionStaticListPtr<DataType, DataList> & rhs) = default;
    NetReflectionStaticListPtr & operator = (const NetReflectionStaticListPtr<DataType, DataList> & rhs) = default;
    
    NetReflectionStaticListPtr & operator = (const DataType & val)
    {
        return operator = (&val);
    }

    NetReflectionStaticListPtr & operator = (const DataType * val)
    {
        for(int index = 0; index < static_cast<int>(DataList->size()); ++index)
        {
            auto elem = &DataList->at(index);
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
        return DataList->at(m_CurrentIndex);
    }

    const DataType * operator -> () const
    {
        return &DataList->at(m_CurrentIndex);
    }

    operator const DataType * () const
    {
        return &DataList->at(m_CurrentIndex);
    }

    const DataType & Value() const
    {
        return DataList->at(m_CurrentIndex);
    }

    int CurrentIndex() const
    {
        return m_CurrentIndex;
    }

    bool operator == (const NetReflectionStaticListPtr<DataType, DataList> & rhs) const
    {
        return m_CurrentIndex == rhs.m_CurrentIndex;
    }

private:
    int m_CurrentIndex = 0;
};

template <typename DataType, const std::vector<DataType> * DataList, class NetBitWriter>
struct NetSerializer<NetReflectionStaticListPtr<DataType, DataList>, NetBitWriter>
{
  void operator()(const NetReflectionStaticListPtr<DataType, DataList> & val, NetBitWriter & writer)
  {
      auto required_bits = GetRequiredBits(DataList->size());
      writer.WriteBits(val.CurrentIndex(), required_bits);
  }
};

template <typename DataType, const std::vector<DataType> * DataList, class NetBitReader>
struct NetDeserializer<NetReflectionStaticListPtr<DataType, DataList>, NetBitReader>
{
  void operator()(NetReflectionStaticListPtr<DataType, DataList> & val, NetBitReader & reader)
  {
      auto required_bits = GetRequiredBits(DataList->size());
      val = DataList->at(reader.ReadUBits(required_bits));
  }
};
