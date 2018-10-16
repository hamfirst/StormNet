#pragma once

#include <cstdint>
#include <cstddef>
#include <typeinfo>

template <typename T>
struct StormReflTypeInfo;

class NetBitReader
{
public:

  virtual uint64_t ReadUBits(int num_bits) = 0;
  virtual int64_t ReadSBits(int num_bits) = 0;

  virtual void ReadBuffer(void * buffer, std::size_t num_bytes);
  virtual bool IsEmpty() = 0;

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

protected:
  int64_t SignExtend(uint64_t val, int bits);
};
