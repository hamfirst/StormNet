#pragma once

#include <vector>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

template <class T, std::size_t MaxSize>
class NetArrayList
{
public:

  using value_type = T;

  NetArrayList() = default;
  NetArrayList(const NetArrayList<T, MaxSize> & rhs) = default;
  NetArrayList(NetArrayList<T, MaxSize> && rhs) = default;

  NetArrayList<T, MaxSize> & operator = (const NetArrayList<T, MaxSize> & rhs) = default;
  NetArrayList<T, MaxSize> & operator = (NetArrayList<T, MaxSize> && rhs) = default;

  bool operator == (const NetArrayList<T, MaxSize> & rhs) const
  {
    if (m_Values.size() != rhs.m_Values.size())
    {
      return false;
    }

    auto itr1 = begin();
    auto itr2 = rhs.begin();

    auto last = end();

    while (itr1 != last)
    {
      if (*itr1 == *itr2)
      {
        ++itr1;
        ++itr2;
        continue;
      }

      return false;
    }

    return true;
  }

  void PushBack(const T & val)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    m_Values.push_back(val);
  }

  template <class... Args>
  T & EmplaceBack(Args &&... args)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    m_Values.emplace_back(std::forward<Args>(args)...);
    return m_Values[m_Values.size() - 1];
  }

  T & InsertAt(const T & val, std::size_t logical_index)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    if (logical_index == m_Values.size())
    {
      m_Values.push_back(val);
      return;
    }

    m_Values.insert(m_Values.begin() + logical_index, val);
    return m_Values[logical_index];
  }

  template <class... Args>
  T & EmplaceAt(std::size_t logical_index, Args &&... args)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    if (logical_index == m_Values.size())
    {
      m_Values.emplace_back(std::forward<Args>(args)...);
      return;
    }

    m_Values.emplace(m_Values.begin() + logical_index, std::forward<Args>(args)...);
    return m_Values[logical_index];
  }

  void RemoveAt(std::size_t logical_index)
  {
    m_Values.erase(m_Values.begin() + logical_index);
  }

  auto & operator [] (std::size_t index)
  {
    return m_Values[index];
  }

  auto & operator [] (std::size_t index) const
  {
    return m_Values[index];
  }

  auto begin()
  {
    return m_Values.begin();
  }

  auto end()
  {
    return m_Values.end();
  }

  auto begin() const
  {
    return m_Values.begin();
  }

  auto end() const
  {
    return m_Values.end();
  }

  void push_back(const T & val)
  {
    PushBack(val);
  }

  auto size() const
  {
    return m_Values.size();
  }

  void resize(std::size_t new_size)
  {
    if (new_size >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    m_Values.resize(new_size);
  }

  void clear()
  {
    m_Values.clear();
  }

  template <class... Args>
  auto emplace_back(Args &&... args)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    return m_Values.emplace(std::forward<Args>(args)...);
  }
  
  template <class Itr>
  auto insert(Itr pos, const T & val)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    return m_Values.insert(pos, val);
  }

  template <class Itr, class... Args>
  auto emplace(Itr pos, Args&&... args)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetArrayList overflow"));
    }

    return m_Values.emplace(pos, std::forward<Args>(args)...);
  }

private:

  std::vector<T> m_Values;
};

template <typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializer<NetArrayList<T, MaxSize>, NetBitWriter>
{
  void operator()(const NetArrayList<T, MaxSize> & val, NetBitWriter & writer)
  {
    auto size = val.size();
    writer.WriteBits(size, GetRequiredBits(MaxSize));

    for (auto & elem : val)
    {
      NetSerializeValue(elem, writer);
    }
  }
};

template <typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializerDelta<NetArrayList<T, MaxSize>, NetBitWriter>
{
  bool operator()(const NetArrayList<T, MaxSize> & val, const NetArrayList<T, MaxSize> & compare, NetBitWriter & writer)
  {
    auto required_bits = GetRequiredBits(MaxSize);

    if (val.size() != compare.size())
    {
      writer.WriteBits(1, 1);
      writer.WriteBits(val.size(), required_bits);

      for (auto index = 0; index < val.size(); index++)
      {
        if (index >= compare.size())
        {
          NetSerializeValue(val[index], writer);
        }
        else
        {
          auto cursor = writer.Reserve(1);
          if (NetSerializeValueDelta(val[index], compare[index], writer))
          {
            cursor.WriteBits(1, 1);
          }
          else
          {
            cursor.WriteBits(0, 1);
          }
        }
      }

      return true;
    }

    auto begin_cursor = writer.Reserve(1);
    int num_wrote = 0;

    for (auto index = 0; index < val.size(); index++)
    {
      auto cursor = writer.Reserve(1);
      if (NetSerializeValueDelta(val[index], compare[index], writer))
      {
        cursor.WriteBits(1, 1);
        num_wrote++;
      }
      else
      {
        cursor.WriteBits(0, 1);
      }
    }

    if (num_wrote > 0)
    {
      begin_cursor.WriteBits(0, 1);
      return true;
    }

    writer.RollBack(begin_cursor);
    return false;
  }
};

template <typename T, std::size_t MaxSize, class NetBitReader>
struct NetDeserializer<NetArrayList<T, MaxSize>, NetBitReader>
{
  void operator()(NetArrayList<T, MaxSize> & val, NetBitReader & reader)
  {
    auto size = reader.ReadUBits(GetRequiredBits(MaxSize));
    while (val.size() < size)
    {
      val.EmplaceBack();
    }

    while (val.size() > size)
    {
      val.RemoveAt(val.size() - 1);
    }

    for (std::size_t index = 0; index < size; index++)
    {
      NetDeserializeValue(val[index], reader);
    }
  }
};

template <typename T, std::size_t MaxSize, class NetBitReader>
struct NetDeserializerDelta<NetArrayList<T, MaxSize>, NetBitReader>
{
  void operator()(NetArrayList<T, MaxSize> & val, NetBitReader & reader)
  {
    auto required_bits = GetRequiredBits(MaxSize);
    auto size_change = reader.ReadUBits(1);

    if (size_change)
    {
      auto new_size = reader.ReadUBits(required_bits);
      auto min_size = std::min(val.size(), new_size);

      for (auto index = 0; index < min_size; index++)
      {
        auto val_changed = reader.ReadUBits(1);
        if (val_changed)
        {
          NetDeserializeValueDelta(val[index], reader);
        }
      }

      for (int64_t index = (int64_t)val.size() - 1; index >= (int64_t)min_size; index--)
      {
        val.RemoveAt(index);
      }

      for (auto index = val.size(); index < new_size; index++)
      {
        val.EmplaceBack();
        NetDeserializeValue(val[index], reader);
      }

      return;
    }

    for (auto index = 0; index < val.size(); index++)
    {
      auto val_changed = reader.ReadUBits(1);
      if (val_changed)
      {
        NetDeserializeValueDelta(val[index], reader);
      }
    }
  }
};

