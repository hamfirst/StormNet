#pragma once 

#include <vector>
#include <cstdlib>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

template <class A, class B> struct NetSparseListPairHelper
{
  NetSparseListPairHelper(std::pair<A, B> && pair_data) :
    m_Data(pair_data)
  {

  }

  template <class AA, class BB>
  NetSparseListPairHelper(AA && a, BB && b) :
    m_Data(a, b)
  {
  }

  NetSparseListPairHelper(NetSparseListPairHelper<A, B> && rhs) = default;

  std::pair<A, B> * operator ->()
  {
    return &m_Data;
  }

private:
  std::pair<A, B> m_Data;
};


template <class T, std::size_t MaxSize>
class NetSparseList
{
public:
  using ContainerType = T;

  template <typename PtrType, typename RefType, typename ListPtrType>
  struct NetSparseListIterator
  {
    NetSparseListIterator(const NetSparseListIterator<PtrType, RefType, ListPtrType> & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator != (const NetSparseListIterator<PtrType, RefType, ListPtrType> & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    bool operator == (const NetSparseListIterator<PtrType, RefType, ListPtrType> & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return false;
      }

      return m_PhysicalIndex == rhs.m_PhysicalIndex;
    }

    std::pair<std::size_t, RefType> operator *() const
    {
      return std::pair<std::size_t, RefType>((std::size_t)m_PhysicalIndex, m_List->m_Values[m_PhysicalIndex].m_Value);
    }

    NetSparseListPairHelper<std::size_t, RefType> operator ->() const
    {
      return NetSparseListPairHelper<std::size_t, RefType>((std::size_t)m_PhysicalIndex, m_List->m_Values[m_PhysicalIndex].m_Value);
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex <= m_List->m_HighestIndex && m_List->m_Values[m_PhysicalIndex].m_Valid == false);
    }

  private:

    NetSparseListIterator(ListPtrType list, int physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    int m_PhysicalIndex;
    ListPtrType m_List;

    friend class NetSparseList<T, MaxSize>;
  };

  using iterator = NetSparseListIterator<T *, T &, NetSparseList<T, MaxSize> *>;
  using const_iterator = NetSparseListIterator<const T *, const T &, const NetSparseList<T, MaxSize> *>;

  NetSparseList() :
    m_HighestIndex(-1),
    m_Capacity(0),
    m_Values(nullptr)
  {

  }

  NetSparseList(const NetSparseList<T, MaxSize> & rhs) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity)
  {
    if (rhs.m_Capacity > 0)
    {
      m_Values = Allocate<ContainerData>(rhs.m_Capacity);

      for (std::size_t index = 0; index < rhs.m_Capacity; index++)
      {
        m_Values[index].m_Valid = rhs.m_Values[index].m_Valid;
        if (rhs.m_Values[index].m_Valid)
        {
          new(&m_Values[index].m_Value) T(rhs.m_Values[index].m_Value);
        }
      }
    }
    else
    {
      m_Values = nullptr;
    }
  }

  NetSparseList(NetSparseList<T, MaxSize> && rhs) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity),
    m_Values(rhs.m_Values)
  {
    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
    rhs.m_Values = nullptr;

  }


  ~NetSparseList()
  {
    DestroyAllElements();

    if (m_Capacity > 0)
    {
      Deallocate(m_Values);
    }
  }

  NetSparseList<T, MaxSize> & operator = (const NetSparseList<T, MaxSize> & rhs)
  {
    Copy(rhs);

    return *this;
  }

  NetSparseList<T, MaxSize> & operator = (NetSparseList<T, MaxSize> && rhs)
  {
    Move(std::move(rhs));

    return *this;
  }


  void Clear()
  {
    DestroyAllElementsAndInvalidate();
    m_HighestIndex = -1;
  }

  void Reserve(std::size_t size)
  {
    if (size > m_Capacity)
    {
      Grow(size);
    }
  }

  template <typename ... InitArgs>
  std::size_t Emplace(InitArgs && ... args)
  {
    for (std::size_t index = 0; index < m_Capacity; ++index)
    {
      if (m_Values[index].m_Valid == false)
      {
        EmplaceAt(index, std::forward<InitArgs>(args)...);
        return index;
      }
    }
    EmplaceBack(std::forward<InitArgs>(args)...);
    return (std::size_t)m_HighestIndex;
  }

  T & PushBack(const T & val)
  {
    if (m_HighestIndex + 1 == m_Capacity)
    {
      Grow();
    }

    m_HighestIndex++;

    m_Values[m_HighestIndex].m_Valid = true;
    new(&m_Values[m_HighestIndex].m_Value) T(val);

    return m_Values[m_HighestIndex].m_Value;
  }

  template <typename ... InitArgs>
  T & EmplaceBack(InitArgs && ... args)
  {
    if (m_HighestIndex + 1 == m_Capacity)
    {
      Grow();
    }

    m_HighestIndex++;

    m_Values[m_HighestIndex].m_Valid = true;
    new(&m_Values[m_HighestIndex].m_Value) T(std::forward<InitArgs>(args)...);

    return m_Values[m_HighestIndex].m_Value;
  }

  T & InsertAt(std::size_t logical_index, const T & val)
  {
    GrowToFit(logical_index);

    m_HighestIndex = m_HighestIndex == -1 || logical_index > (std::size_t)m_HighestIndex ? (int)logical_index : m_HighestIndex;

    if (m_Values[logical_index].m_Valid)
    {
      m_Values[logical_index].m_Value = val;
    }
    else
    {
      m_Values[logical_index].m_Valid = true;
      new (&m_Values[logical_index].m_Value) T(val);
    }

    return m_Values[logical_index].m_Value;
  }

  template <typename ... InitArgs>
  T & EmplaceAt(std::size_t logical_index, InitArgs && ... args)
  {
    GrowToFit(logical_index);

    m_HighestIndex = m_HighestIndex == -1 || logical_index > (std::size_t)m_HighestIndex ? (int)logical_index : m_HighestIndex;

    if (m_Values[logical_index].m_Valid)
    {
      m_Values[logical_index].m_Value = T(std::forward<InitArgs>(args)...);
    }
    else
    {
      m_Values[logical_index].m_Valid = true;
      new (&m_Values[logical_index].m_Value) T(std::forward<InitArgs>(args)...);
    }

    return m_Values[logical_index].m_Value;
  }

  void Remove(const iterator & itr)
  {
    RemoveAt(itr.m_PhysicalIndex);
  }

  void Remove(const const_iterator & itr)
  {
    RemoveAt(itr.m_PhysicalIndex);
  }

  void RemoveAt(std::size_t logical_index)
  {
    if (logical_index >= m_Capacity)
    {
      return;
    }

    if (m_Values[logical_index].m_Valid)
    {
      m_Values[logical_index].m_Value.~T();
      m_Values[logical_index].m_Valid = false;
      while (m_HighestIndex >= 0 && m_Values[m_HighestIndex].m_Valid == false)
      {
        m_HighestIndex--;
      }
    }
  }

  int HighestIndex() const
  {
    return m_HighestIndex;
  }

  void Compress()
  {
    int empty_index = -1;
    for (std::size_t index = 0; index < m_Capacity; index++)
    {
      if (m_Values[index].m_Valid)
      {
        if (empty_index != -1)
        {
          new (&m_Values[empty_index].m_Value) T(std::move(m_Values[index].m_Value));
          m_Values[index].m_Value.~T();
          m_Values[empty_index].m_Valid = true;
          m_Values[index].m_Valid = false;

          empty_index++;
        }
      }
      else
      {
        if (empty_index == -1)
        {
          empty_index = (int)index;
        }
      }
    }
  }

  bool HasAt(std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      return false;
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      return false;
    }

    if (m_Values[index].m_Valid == false)
    {
      return false;
    }

    return true;
  }

  T & operator[](std::size_t index)
  {
    if (m_HighestIndex == -1)
    {
      throw std::out_of_range("Invalid index");
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      throw std::out_of_range("Invalid index");
    }

    if (m_Values[index].m_Valid == false)
    {
      throw std::out_of_range("Invalid index");
    }

    return m_Values[index].m_Value;
  }

  const T & operator[](std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      throw std::out_of_range("Invalid index");
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      throw std::out_of_range("Invalid index");
    }

    if (m_Values[index].m_Valid == false)
    {
      throw std::out_of_range("Invalid index");
    }

    return m_Values[index].m_Value;
  }

  T & GetAt(std::size_t index)
  {
    if (m_HighestIndex == -1)
    {
      throw std::out_of_range("Invalid index");
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      throw std::out_of_range("Invalid index");
    }

    if (m_Values[index].m_Valid == false)
    {
      throw std::out_of_range("Invalid index");
    }

    return m_Values[index].m_Value;
  }

  const T & GetAt(std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      throw std::out_of_range("Invalid index");
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      throw std::out_of_range("Invalid index");
    }

    if (m_Values[index].m_Valid == false)
    {
      throw std::out_of_range("Invalid index");
    }

    return m_Values[index].m_Value;
  }

  T * TryGet(std::size_t index)
  {
    if (m_HighestIndex == -1)
    {
      return nullptr;
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      return nullptr;
    }

    if (m_Values[index].m_Valid == false)
    {
      return nullptr;
    }

    return &m_Values[index].m_Value;
  }

  const T * TryGet(std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      return nullptr;
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      return nullptr;
    }

    if (m_Values[index].m_Valid == false)
    {
      return nullptr;
    }

    return &m_Values[index].m_Value;
  }

  std::size_t Size() const
  {
    std::size_t count = 0;
    for (int index = 0; index <= m_HighestIndex; ++index)
    {
      if (m_Values[index].m_Valid)
      {
        count++;
      }
    }

    return count;
  }

  std::size_t Capacity() const
  {
    return m_Capacity;
  }

  iterator begin()
  {
    int start_index = 0;
    while (start_index <= m_HighestIndex && m_Values[start_index].m_Valid == false)
    {
      start_index++;
    }

    iterator itr(this, start_index);
    return itr;
  }

  iterator end()
  {
    iterator itr(this, m_HighestIndex + 1);
    return itr;
  }

  const_iterator begin() const
  {
    int start_index = 0;
    while (start_index <= m_HighestIndex && m_Values[start_index].m_Valid == false)
    {
      start_index++;
    }

    const_iterator itr(this, start_index);
    return itr;
  }

  const_iterator end() const
  {
    const_iterator itr(this, m_HighestIndex + 1);
    return itr;
  }

  bool operator == (const NetSparseList<T, MaxSize> & rhs) const
  {
    if (m_HighestIndex != rhs.m_HighestIndex)
    {
      return false;
    }

    auto itr1 = begin();
    auto itr2 = rhs.begin();

    auto last = end();

    while (itr1 != last)
    {
      if (itr1->first == itr2->first && itr1->second == itr2->second)
      {
        ++itr1;
        ++itr2;
        continue;
      }

      return false;
    }

    return true;
  }

private:

  template <typename Elem>
  Elem * Allocate(std::size_t count)
  {
    auto ptr = (Elem *)malloc(sizeof(Elem) * count);
    if (ptr == nullptr)
    {
      throw std::bad_alloc();
    }

    return ptr;
  }

  void Deallocate(void * ptr)
  {
    free(ptr);
  }

  void Grow()
  {
    if (m_Capacity == 0)
    {
      Grow(1);
      return;
    }

    Grow(m_Capacity * 2);
  }

  void GrowToFit(std::size_t logical_index)
  {
    if (logical_index >= m_Capacity)
    {
      if (m_Capacity == 0)
      {
        Grow(logical_index + 1);
      }
      else
      {
        std::size_t new_capacity = m_Capacity;
        do
        {
          new_capacity *= 2;
        } while (new_capacity <= logical_index);

        Grow(new_capacity);
      }
    }
  }

  void Grow(std::size_t requested_size)
  {
    auto values = Allocate<ContainerData>(requested_size);

    if (m_Capacity > 0)
    {
      for (int index = 0; index <= m_HighestIndex; index++)
      {
        values[index].m_Valid = m_Values[index].m_Valid;
        if (m_Values[index].m_Valid)
        {
          new (&values[index].m_Value) T(std::move(m_Values[index].m_Value));
          m_Values[index].m_Value.~T();
        }
      }

      Deallocate(m_Values);
    }

    for (int index = m_HighestIndex + 1; index < (int)requested_size; index++)
    {
      values[index].m_Valid = false;
    }

    m_Values = values;
    m_Capacity = requested_size;
  }

  void DestroyAllElements()
  {
    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        m_Values[index].m_Value.~T();
      }
    }
  }

  void DestroyAllElementsAndInvalidate()
  {
    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        m_Values[index].m_Value.~T();
        m_Values[index].m_Valid = false;
      }
    }
  }

  void Copy(const NetSparseList<T, MaxSize> & rhs)
  {
    DestroyAllElements();

    if (rhs.m_HighestIndex >= (int)m_Capacity)
    {
      Deallocate(m_Values);

      m_Values = Allocate<ContainerData>(rhs.m_Capacity);
      m_Capacity = rhs.m_Capacity;
    }

    for (int index = 0; index <= rhs.m_HighestIndex; index++)
    {
      m_Values[index].m_Valid = rhs.m_Values[index].m_Valid;
      if (rhs.m_Values[index].m_Valid)
      {
        new(&m_Values[index].m_Value) T(rhs.m_Values[index].m_Value);
      }
    }

    for (int index = rhs.m_HighestIndex + 1; index < (int)m_Capacity; index++)
    {
      m_Values[index].m_Valid = false;
    }

    m_HighestIndex = rhs.m_HighestIndex;
  }

  void Move(NetSparseList<T, MaxSize> && rhs)
  {
    DestroyAllElements();

    if (m_Capacity > 0)
    {
      Deallocate(m_Values);
    }

    m_HighestIndex = rhs.m_HighestIndex;
    m_Capacity = rhs.m_Capacity;
    m_Values = rhs.m_Values;

    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
    rhs.m_Values = nullptr;
  }

  struct ContainerData
  {
    bool m_Valid;
    T m_Value;
  };

  int m_HighestIndex;
  std::size_t m_Capacity;
  ContainerData * m_Values;
};



template <typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializer<NetSparseList<T, MaxSize>, NetBitWriter>
{
  void operator()(const NetSparseList<T, MaxSize> & val, NetBitWriter & writer)
  {
    auto size = val.HighestIndex();

    std::size_t num_elements = 0;
    auto key_bits = GetRequiredBits(MaxSize);
    auto size_cursor = writer.Reserve(key_bits);

    for (int index = 0; index <= size; index++)
    {
      if (val.HasAt(index))
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
  bool operator()(const NetSparseList<T, MaxSize> & to, const NetSparseList<T, MaxSize> & from, NetBitWriter & writer)
  {
    auto begin_cursor = writer.Reserve(1);
    bool size_change = (to.HighestIndex() != from.HighestIndex());
    if (size_change)
    {
      begin_cursor.WriteBits(1, 1);
      writer.WriteBits(to.HighestIndex() + 1, GetRequiredBits(MaxSize));
    }

    int num_wrote = 0;
    int required_bits = GetRequiredBits(to.HighestIndex());

    int required_size_bits = GetRequiredBits(to.HighestIndex() + 1);
    auto size_cursor = writer.Reserve(required_size_bits);

    for (int index = 0; index <= to.HighestIndex(); index++)
    {
      auto index_cursor = writer.Reserve(required_bits);

      if (to.HasAt(index) && from.HasAt(index) == false)
      {
        index_cursor.WriteBits(index, required_bits);
        NetSerializeValue(to[index], writer);
        num_wrote++;
      }
      else if (to.HasAt(index) == false && from.HasAt(index))
      {
        index_cursor.WriteBits(index, required_bits);
        writer.WriteBits(0, 1);
        num_wrote++;
      }
      else
      {
        auto new_elem_cursor = writer.Reserve(1);
        if (to.HasAt(index) == false || NetSerializeValueDelta(to[index], from[index], writer) == false)
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

    size_cursor.WriteBits(num_wrote, required_size_bits);
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

    int current_index = 0;

    while (num_elements > 0)
    {
      int input_index = (int)reader.ReadUBits(key_bits);
      while (current_index < input_index)
      {
        val.RemoveAt(current_index);
        current_index++;
      }

      if (!val.HasAt(current_index))
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
    int highest_index = size_change ? (int)(reader.ReadUBits(GetRequiredBits(MaxSize)) - 1) : val.HighestIndex();

    int required_bits = GetRequiredBits(highest_index);
    auto size = reader.ReadUBits(GetRequiredBits(highest_index + 1));

    for (auto elem = 0; elem < size; elem++)
    {
      auto index = reader.ReadUBits(required_bits);

      if (val.HasAt((std::size_t)index) == false)
      {
        val.EmplaceAt((std::size_t)index);
        NetDeserializeValue(val[(std::size_t)index], reader);
      }
      else
      {
        auto existing_elem = reader.ReadUBits(1);
        if (existing_elem)
        {
          NetDeserializeValueDelta(val[(std::size_t)index], reader);
        }
        else
        {
          val.RemoveAt((std::size_t)index);
        }
      }
    }

    for (auto index = val.HighestIndex(); index > highest_index; index--)
    {
      val.RemoveAt(index);
    }
  }
};


