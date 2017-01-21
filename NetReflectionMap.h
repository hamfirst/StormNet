#pragma once

#include <map>

#include "NetReflectionCommon.h"
#include "NetException.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

template <class K, class T, std::size_t MaxSize>
class NetMap
{
public:

  NetMap() = default;
  NetMap(const NetMap<K, T, MaxSize> & rhs) = default;
  NetMap(NetMap<K, T, MaxSize> && rhs) = default;

  NetMap<K, T, MaxSize> & operator = (const NetMap<K, T, MaxSize> & rhs) = default;
  NetMap<K, T, MaxSize> & operator = (NetMap<K, T, MaxSize> && rhs) = default;

  bool operator == (const NetMap<K, T, MaxSize> & rhs) const
  {
    if (rhs.Size() != this->Size())
    {
      return false;
    }

    auto it1 = rhs.begin();
    auto it2 = this->begin();

    auto last = this->end();
    while (it2 != last)
    {
      if (it1->first == it2->first && it1->second == it2->second)
      {
        ++it1;
        ++it2;
        continue;
      }

      return false;
    }

    return true;
  }

  void Clear()
  {
    m_Values.clear();
  }

  void Reserve(size_t size)
  {
    m_Values.reserve(size);
  }

  void Set(const K & k, T && t)
  {
    if (m_Values.size() >= MaxSize)
    {
      NET_THROW(std::runtime_error("NetReflectionMap overflow"));
    }

    m_Values.insert_or_assign(k, t);
  }

  template <class... Args>
  T & EmplaceAt(const K & k, Args &&... args)
  {
    auto emplace_info = m_Values.try_emplace(k, std::forward<Args>(args)...);
    return (emplace_info.first)->second;
  }

  void Remove(const K & k)
  {
    auto itr = m_Values.find(k);
    if (itr == m_Values.end())
    {
      return;
    }

    m_Values.erase(itr);
  }

  template <class Iterator>
  void Erase(Iterator & itr)
  {
    m_Values.erase(itr);
  }

  std::experimental::optional<T &> Get(const K & k)
  {
    auto itr = m_Values.find(k);
    if (itr == m_Values.end())
    {
      return std::experimental::optional<T &>();
    }

    return itr->second;
  }

  const std::experimental::optional<T &> Get(const K & k) const
  {
    auto itr = m_Values.find(k);
    if (itr == m_Values.end())
    {
      return std::experimental::optional<T &>();
    }

    return itr->second;
  }

  T & operator [] (const K & k)
  {
    return m_Values[k];
  }

  const T & operator [] (const K & k) const
  {
    return m_Values[k];
  }

  std::size_t Size() const
  {
    return m_Values.size();
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

private:
  std::map<K, T> m_Values;
};

template <typename K, typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializer<NetMap<K, T, MaxSize>, NetBitWriter>
{
  void operator()(const NetMap<K, T, MaxSize> & val, NetBitWriter & writer)
  {
    auto size = val.Size();

    int key_bits = GetRequiredBits(MaxSize);
    writer.WriteBits(size, key_bits);

    for (auto & elem : val)
    {
      writer.WriteBits(elem.first, key_bits);
      NetSerializeValue(elem.second, writer);
    }
  }
};

template <typename K, typename T, std::size_t MaxSize, class NetBitWriter>
struct NetSerializerDelta<NetMap<K, T, MaxSize>, NetBitWriter>
{
  bool operator()(const NetMap<K, T, MaxSize > & to, const NetMap<K, T, MaxSize> & from, NetBitWriter & writer)
  {
    auto itr = to.begin();
    auto other_itr = from.begin();

    int required_bits = GetRequiredBits(MaxSize);
    int num_wrote = 0; 

    auto size_cursor = writer.Reserve(required_bits);

    while (itr != to.end())
    {
      while (other_itr != from.end() && other_itr->first < itr->first)
      {
        writer.WriteBits(other_itr->first, required_bits);
        writer.WriteBits(0, 1);

        ++other_itr;
        num_wrote++;
      }

      while (itr != to.end() && other_itr != from.end() && other_itr->first == itr->first)
      {
        auto index_cursor = writer.Reserve(required_bits + 1);
        if (NetSerializeValueDelta(itr->second, other_itr->second, writer))
        {
          index_cursor.WriteBits(itr->first, required_bits);
          index_cursor.WriteBits(1, 1);
          num_wrote++;
        }
        else
        {
          writer.RollBack(index_cursor);
        }

        ++itr;
        ++other_itr;
      }

      while (itr != to.end() && (other_itr == from.end() || itr->first < other_itr->first))
      {
        writer.WriteBits(itr->first, required_bits);
        NetSerializeValue(itr->second, writer);

        ++itr;
        num_wrote++;
      }
    }

    while (other_itr != from.end())
    {
      writer.WriteBits(other_itr->first, required_bits);
      writer.WriteBits(0, 1);

      ++other_itr;
      num_wrote++;
    }

    if (num_wrote == 0)
    {
      writer.RollBack(size_cursor);
      return false;
    }

    size_cursor.WriteBits(num_wrote, required_bits);
    return true;
  }
};

template <typename K, typename T, std::size_t MaxSize, class NetBitReader>
struct NetDeserializer<NetMap<K, T, MaxSize>, NetBitReader>
{
  void operator()(NetMap<K, T, MaxSize> & val, NetBitReader & reader)
  {
    auto key_bits = GetRequiredBits(MaxSize);
    auto num_elements = reader.ReadUBits(key_bits);

    auto current_itr = val.begin();

    while (num_elements > 0)
    {
      std::size_t input_index = reader.ReadUBits(key_bits);
      while (current_itr != val.end() && current_itr->first < input_index)
      {
        auto dead_itr = current_itr;
        ++current_itr;

        val.Erase(dead_itr);
      }

      if (current_itr == val.end() || current_itr->first != input_index)
      {
        auto & elem = val.EmplaceAt(static_cast<K>(input_index));
        NetDeserializeValue(elem, reader);
      }
      else
      {
        NetDeserializeValue(current_itr->second, reader);
        ++current_itr;
      }

      num_elements--;
    }

    while (current_itr != val.end())
    {
      auto dead_itr = current_itr;
      current_itr++;

      val.Erase(dead_itr);
    }
  }
};

template <typename K, typename T, std::size_t MaxSize, class NetBitReader>
struct NetDeserializerDelta<NetMap<K, T, MaxSize>, NetBitReader>
{
  void operator()(NetMap<K, T, MaxSize > & val, NetBitReader & reader)
  {
    int required_bits = GetRequiredBits(MaxSize);
    auto size = reader.ReadUBits(required_bits);

    for (auto elem = 0; elem < size; elem++)
    {
      auto elem_index = reader.ReadUBits(required_bits);
      auto elem_val = val.Get(static_cast<K>(elem_index));

      if (elem_val)
      {
        auto existing_elem = reader.ReadUBits(1);
        if (existing_elem)
        {
          NetDeserializeValueDelta(*elem_val, reader);
        }
        else
        {
          val.Remove(static_cast<K>(elem_index));
        }
      }
      else
      {
        auto & elem_val = val.EmplaceAt(static_cast<K>(elem_index));
        NetDeserializeValue(elem_val, reader);
      }
    }
  }
};

