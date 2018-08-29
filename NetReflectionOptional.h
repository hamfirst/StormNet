#pragma once

template <class T>
class NetOptional
{
public:
  using MyType = T;

  NetOptional() :
    m_Valid(false)
  {

  }

  NetOptional(const NetOptional<T> & rhs)
  {
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      const T * src = reinterpret_cast<const T *>(rhs.m_Buffer);
      new(m_Buffer) T(*src);
    }
  }

  NetOptional(NetOptional<T> && rhs)
  {
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(rhs.m_Buffer);
      new(m_Buffer) T(std::move(*src));
      rhs.Clear();
    }
  }

  NetOptional(const T & rhs) :
    m_Valid(true)
  {
    new(m_Buffer) T(rhs);
  }

  NetOptional(T && rhs) :
    m_Valid(true)
  {
    new(m_Buffer) T(std::move(rhs));
  }

  NetOptional<T> & operator =(const NetOptional<T> & rhs)
  {
    Clear();
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      const T * src = reinterpret_cast<const T *>(rhs.m_Buffer);
      new(m_Buffer) T(*src);
    }

    return *this;
  }

  NetOptional<T> & operator =(NetOptional<T> && rhs)
  {
    Clear();
    m_Valid = rhs.m_Valid;
    if (m_Valid)
    {
      T * src = reinterpret_cast<T *>(rhs.m_Buffer);
      new(m_Buffer) T(std::move(*src));
      rhs.Clear();
    }
    return *this;
  }

  ~NetOptional()
  {
    Clear();
  }

  template <typename ... Args>
  T & Emplace(Args && ... args)
  {
    Clear();

    T * src = new(m_Buffer) T(std::forward<Args>(args)...);
    m_Valid = true;
    return *src;
  }

  void Clear()
  {
    if (m_Valid)
    {
      T * ptr = reinterpret_cast<T *>(m_Buffer);
      ptr->~T();
    }

    m_Valid = false;
  }

  operator bool() const
  {
    return m_Valid;
  }

  bool IsValid() const
  {
    return m_Valid;
  }

  T & Value()
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<T *>(m_Buffer);
  }

  const T & Value() const
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<const T *>(m_Buffer);
  }

  T & operator *()
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<T *>(m_Buffer);
  }

  const T & operator *() const
  {
    if (!m_Valid)
    {
      throw false;
    }

    return *reinterpret_cast<const T *>(m_Buffer);
  }

  T * operator ->()
  {
    if (!m_Valid)
    {
      throw false;
    }

    return reinterpret_cast<T *>(m_Buffer);
  }

  const T * operator ->() const
  {
    if (!m_Valid)
    {
      throw false;
    }

    return reinterpret_cast<const T *>(m_Buffer);
  }

  T * GetPtr()
  {
    if (!m_Valid)
    {
      return nullptr;
    }

    return reinterpret_cast<T *>(m_Buffer);
  }

  const T * GetPtr() const
  {
    if (!m_Valid)
    {
      return nullptr;
    }

    return reinterpret_cast<const T *>(m_Buffer);
  }

public:

  bool m_Valid;
  alignas(alignof(T)) unsigned char m_Buffer[sizeof(T)];

};

template <class T, class NetBitWriter>
struct NetSerializer<NetOptional<T>, NetBitWriter>
{
  void operator()(const NetOptional<T> & val, NetBitWriter & writer)
  {
    if (val)
    {
      writer.WriteBits(1, 1);

      NetSerializer<T, NetBitWriter> serializer;
      serializer(val.Value(), writer);
    }
    else
    {
      writer.WriteBits(0, 1);
    }
  }
};

template <class T, class NetBitReader>
struct NetDeserializer<NetOptional<T>, NetBitReader>
{
  void operator()(NetOptional<T> & val, NetBitReader & reader)
  {
    if (reader.ReadUBits(1))
    {
      if (val == false)
      {
        val.Emplace();
      }

      NetDeserializer<T, NetBitReader> deserializer;
      deserializer(val.Value(), reader);
    }
    else
    {
      val.Clear();
    }
  }
};


