#pragma once

#include <cstddef>
#include <cstdint>
#include <typeinfo>
#include <type_traits>

template <typename T>
struct StormReflTypeInfo;

class NetBitWriterCursor;

class NetBitWriter
{
public:
  virtual void WriteBits(uint64_t val, int num_bits) = 0;
  virtual void WriteSBits(int64_t val, int num_bits) = 0;

  virtual void WriteBits(NetBitWriterCursor & cursor, uint64_t val, int num_bits) = 0;
  virtual void WriteSBits(NetBitWriterCursor & cursor, int64_t val, int num_bits) = 0;

  virtual void WriteBuffer(void * data, std::size_t num_bytes);

  virtual NetBitWriterCursor Reserve(int num_bits) = 0;
  virtual void RollBack(NetBitWriterCursor & cursor) = 0;

  template <typename T>
  const T * GetParentObjectAs()
  {
    auto ptr = m_ParentObjectCast(typeid(T).hash_code(), m_ParentObj);
    return static_cast<const T *>(ptr);
  }

  template <typename T>
  void SetParentObject(const T * t)
  {
    m_ParentObj = t;
    m_ParentObjectCast = static_cast<const void * (*)(std::size_t, const void *)>(
            &StormReflTypeInfo<T>::CastFromTypeIdHash);
  }

  void SetParentObject(const void * ptr, const void * (*cast)(std::size_t, const void *))
  {
    m_ParentObj = ptr;
    m_ParentObjectCast = cast;
  }

  auto GetParentObjectRaw() const { return m_ParentObj; }
  auto GetParentObjectCast() const { return m_ParentObjectCast; }

private:
  const void * m_ParentObj = nullptr;
  const void * (*m_ParentObjectCast)(std::size_t type_id_hash, const void * ptr);
};

class NetBitWriterCursor
{
public:
  NetBitWriterCursor(NetBitWriter * writer, void * buffer, std::size_t byte, int bit, std::size_t size, int total_bits)
    : m_Writer(writer), m_Buffer(buffer), m_Byte(byte), m_Bit(bit), m_Size(size), m_Wrote(false), m_TotalBits(total_bits)
  { }

  NetBitWriterCursor(const NetBitWriterCursor & rhs) = default;
  NetBitWriterCursor(NetBitWriterCursor && rhs) = default;

  NetBitWriterCursor & operator = (const NetBitWriterCursor & rhs) = default;
  NetBitWriterCursor & operator = (NetBitWriterCursor && rhs) = default;

  void WriteBits(uint64_t val, int num_bits) { m_Writer->WriteBits(*this, val, num_bits); }
  void WriteSBits(int64_t val, int num_bits) { m_Writer->WriteSBits(*this, val, num_bits); }

public:
  NetBitWriter * m_Writer;
  void * m_Buffer;
  std::size_t m_Byte;
  int m_Bit;
  std::size_t m_Size;
  bool m_Wrote;
  int m_TotalBits;
};