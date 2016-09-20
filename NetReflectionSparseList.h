#pragma once 

#include <vector>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

template <class T, std::size_t MaxSize>
class NetSparseList
{
public:

  using value_type = T;

  NetSparseList() = default;
  NetSparseList(const NetSparseList<T, MaxSize> & rhs) = default;
  NetSparseList(NetSparseList<T, MaxSize> && rhs) = default;

  NetSparseList<T, MaxSize> & operator = (const NetSparseList<T, MaxSize> & rhs) = default;
  NetSparseList<T, MaxSize> & operator = (NetSparseList<T, MaxSize> && rhs) = default;

  bool operator == (const NetSparseList<T, MaxSize> & rhs) const
  {
    if (m_HighestIndex != m_HighestIndex)
    {
      return false;
    }

    auto itr1 = begin();
    auto itr2 = rhs.begin();

    auto last = end();

    while (itr1 != last)
    {
      if ((*itr1).first == (*itr2).first && (*itr1).second == (*itr2).second)
      {
        ++itr1;
        ++itr2;
        continue;
      }

      return false;
    }

    return true;
  }

  struct NetSparseListIterator
  {
    NetSparseListIterator(const NetSparseListIterator & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator != (const NetSparseListIterator & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    const std::pair<std::size_t, T &> operator *() const
    {
      std::pair<std::size_t, T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    const std::pair<std::size_t, T &> operator ->() const
    {
      std::pair<std::size_t, T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex < m_List->m_Values.size() && static_cast<bool>(m_List->m_Values[m_PhysicalIndex]) == false);
    }

  private:

    NetSparseListIterator(NetSparseList<T, MaxSize> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    std::size_t m_PhysicalIndex = 0;
    NetSparseList<T, MaxSize> * m_List;

    friend class NetSparseList<T, MaxSize>;
  };

  struct NetSparseListIteratorConst
  {
    NetSparseListIteratorConst(const NetSparseListIteratorConst & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator != (const NetSparseListIteratorConst & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    const std::pair<std::size_t, const T &> operator *() const
    {
      std::pair<std::size_t, const T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    const std::pair<std::size_t, const T &> operator ->() const
    {
      std::pair<std::size_t, const T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex < m_List->m_Values.size() && static_cast<bool>(m_List->m_Values[m_PhysicalIndex]) == false);
    }

  private:

    NetSparseListIteratorConst(const NetSparseList<T, MaxSize> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    std::size_t m_PhysicalIndex = 0;
    const NetSparseList<T, MaxSize> * m_List;

    friend class NetSparseList<T, MaxSize>;
  };

  bool HasElementAt(std::size_t logical_index) const
  {
    if (logical_index >= m_Values.size())
    {
      return false;
    }

    return static_cast<bool>(m_Values[logical_index]);
  }

  void Clear()
  {
    m_Values.clear();
    m_HighestIndex = 0;
  }

  void Reserve(std::size_t size)
  {
    m_Values.reserve(size);
  }

  void PushBack(const T & val)
  {
    if (m_HighestIndex + 1 >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetSparseList overflow"));
    }

    m_HighestIndex = m_Values.size();
    m_Values.emplace_back(val);
  }

  template <class... Args>
  void EmplaceBack(Args &&... args)
  {
    if (m_HighestIndex + 1 >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetSparseList overflow"));
    }

    m_HighestIndex = m_Values.size();
    m_Values.emplace_back(std::experimental::in_place, std::forward<Args>(args)...);
  }

  void InsertAt(const T & val, std::size_t logical_index)
  {
    if (logical_index >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetSparseList overflow"));
    }

    m_Values.resize(std::max(m_Values.size(), logical_index + 1));

    m_Values[logical_index] = std::experimental::optional<T>(val);
    m_HighestIndex = std::max(m_HighestIndex, logical_index);
  }

  template <class... Args>
  void EmplaceAt(std::size_t logical_index, Args &&... args)
  {
    if (logical_index >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetSparseList overflow"));
    }

    m_Values.resize(std::max(m_Values.size(), logical_index + 1));

    m_Values[logical_index] = std::experimental::optional<T>(std::experimental::in_place, std::forward<Args>(args)...);
    m_HighestIndex = std::max(m_HighestIndex, logical_index);
  }

  void RemoveAt(std::size_t logical_index)
  {
    if (logical_index >= m_Values.size())
    {
      return;
    }

    m_Values[logical_index] = {};

    if (m_HighestIndex == logical_index)
    {
      for (std::size_t index = m_HighestIndex; index >= 0 && static_cast<bool>(m_Values[index]) == false; index--)
      {
        m_HighestIndex--;
      }
    }
  }

  auto & operator [] (std::size_t index)
  {
    return *m_Values[index];
  }

  auto & operator [] (std::size_t index) const
  {
    return *m_Values[index];
  }

  std::size_t HighestIndex() const
  {
    return m_HighestIndex;
  }

  std::size_t Size() const
  {
    return m_Values.size();
  }

  NetSparseListIterator begin()
  {
    int start_index = 0;
    while (start_index < m_HighestIndex && static_cast<bool>(m_Values[start_index]) == false)
    {
      start_index++;
    }

    NetSparseListIterator itr(this, start_index);
    return itr;
  }

  NetSparseListIterator end()
  {
    NetSparseListIterator itr(this, m_HighestIndex);
    return itr;
  }

  NetSparseListIteratorConst begin() const
  {
    std::size_t start_index = 0;
    while (start_index < m_HighestIndex && static_cast<bool>(m_Values[start_index]) == false)
    {
      start_index++;
    }

    NetSparseListIteratorConst itr(this, start_index);
    return itr;
  }

  NetSparseListIteratorConst end() const
  {
    NetSparseListIteratorConst itr(this, m_HighestIndex);
    return itr;
  }

private:

  std::vector<std::experimental::optional<T>> m_Values;
  std::size_t m_HighestIndex = 0;
};


template <typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializer<NetSparseList<T, MaxSize>, NetBitWriter>
{
  void operator()(const NetSparseList<T, MaxSize> & val, NetBitWriter & writer)
  {
    auto size = val.Size();

    std::size_t num_elements = 0;
    auto key_bits = GetRequiredBits(MaxSize);
    auto size_cursor = writer.Reserve(key_bits);

    for (std::size_t index = 0; index < size; index++)
    {
      if (val.HasElementAt(index))
      {
        writer.WriteBits(index, key_bits);
        NetSerializeValue(val[index], writer);

        num_elements++;
      }
    }

    size_cursor.WriteBits(num_elements, key_bits);
  }
};

template <typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializerDelta<NetSparseList<T, MaxSize>, NetBitWriter>
{
  bool operator()(const NetSparseList<T, MaxSize> & val, const NetSparseList<T, MaxSize> & compare, NetBitWriter & writer)
  {
    auto begin_cursor = writer.Reserve(1);
    bool size_change = (val.HighestIndex() != compare.HighestIndex());
    if (size_change)
    {
      begin_cursor.WriteBits(1, 1);
      writer.WriteBits(val.HighestIndex(), GetRequiredBits(MaxSize));
    }

    int num_wrote = 0;
    int required_bits = GetRequiredBits(val.HighestIndex());

    auto size_cursor = writer.Reserve(required_bits);

    for (auto index = 0; index < val.Size(); index++)
    {
      auto index_cursor = writer.Reserve(required_bits);

      if (val.HasElementAt(index) && compare.HasElementAt(index) == false)
      {
        index_cursor.WriteBits(index, required_bits);
        NetSerializeValue(val[index], writer);
        num_wrote++;
      }
      else if (val.HasElementAt(index) == false && compare.HasElementAt(index))
      {
        index_cursor.WriteBits(index, required_bits);
        writer.WriteBits(0, 1);
        num_wrote++;
      }
      else
      {
        auto new_elem_cursor = writer.Reserve(1);
        if (val.HasElementAt(index) == false || NetSerializeValueDelta(val[index], compare[index], writer) == false)
        {
          writer.RollBack(index_cursor);
        }
        else
        {
          index_cursor.WriteBits(index, required_bits);
          new_elem_cursor.WriteBits(1, 1);
          num_wrote++;
        }
      }
    }

    if (num_wrote == 0 && !size_change)
    {
      writer.RollBack(begin_cursor);
      return false;
    }

    if (!size_change)
    {
      begin_cursor.WriteBits(0, 1);
    }

    size_cursor.WriteBits(num_wrote, required_bits);
    return true;
  }
};


template <typename T, std::size_t MaxSize, class NetBitReader>
struct NetDeserializer<NetSparseList<T, MaxSize>, NetBitReader>
{
  void operator()(NetSparseList<T, MaxSize> & val, NetBitReader & reader)
  {
    auto key_bits = GetRequiredBits(MaxSize);
    auto num_elements = reader.ReadUBits(key_bits);

    std::size_t current_index = 0;

    while (num_elements > 0)
    {
      std::size_t input_index = reader.ReadUBits(key_bits);
      while (current_index < input_index)
      {
        val.RemoveAt(current_index);
        current_index++;
      }

      if (!val.HasElementAt(current_index))
      {
        val.EmplaceAt(current_index);
      }

      NetDeserializeValue(val[current_index], reader);
      current_index++;
      num_elements--;
    }

    while (current_index < val.HighestIndex())
    {
      val.RemoveAt(current_index);
      current_index++;
    }
  }
};


template <typename T, std::size_t MaxSize, class NetBitReader>
struct NetDeserializerDelta<NetSparseList<T, MaxSize>, NetBitReader>
{
  void operator()(NetSparseList<T, MaxSize> & val, NetBitReader & reader)
  {
    auto size_change = reader.ReadUBits(1);
    auto highest_index = size_change ? reader.ReadUBits(GetRequiredBits(MaxSize)) : val.HighestIndex();

    int required_bits = GetRequiredBits(highest_index);
    auto size = reader.ReadUBits(required_bits);

    for (auto elem = 0; elem < size; elem++)
    {
      auto index = reader.ReadUBits(required_bits);

      if (val.HasElementAt(index) == false)
      {
        val.EmplaceAt(index);
        NetDeserializeValue(val[index], reader);
      }
      else
      {
        auto existing_elem = reader.ReadUBits(1);
        if (existing_elem)
        {
          NetDeserializeValueDelta(val[index], reader);
        }
        else
        {
          val.RemoveAt(index);
        }
      }
    }
  }
};


