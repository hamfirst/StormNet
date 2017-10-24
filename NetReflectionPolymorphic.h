
#pragma once

#include "NetReflectionCommon.h"
#include "NetReflectionTypeDatabase.h"
#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"

#include <type_traits>

template <class BaseClass>
class NetPolymorphic
{
public:
  NetPolymorphic()
  {
    m_Ptr = static_cast<BaseClass *>(BaseClass::__s_TypeDatabase.GetDefaultTypeInfo().m_HeapCreate());
    m_ClassId = BaseClass::__s_TypeDatabase.GetDefaultClassId();
    m_TypeHash = typeid(BaseClass).hash_code();
  }

  NetPolymorphic(const NetPolymorphic<BaseClass> & rhs)
  {
    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(rhs.m_ClassId);
    m_Ptr = static_cast<BaseClass *>(type_info.m_HeapCreate());
    m_ClassId = rhs.m_ClassId;
    m_TypeHash = rhs.m_TypeHash;

    type_info.m_Copy(m_Ptr, rhs.m_Ptr);
  }

  NetPolymorphic(std::size_t class_id, const void * data_ptr)
  {
    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(class_id);
    m_Ptr = static_cast<BaseClass *>(type_info.m_HeapCreate());
    m_ClassId = class_id;
    m_TypeHash = type_info.m_TypeIdHash;

    type_info.m_Copy(m_Ptr, data_ptr);
  }

  template <typename DataType, std::enable_if_t<std::is_base_of<BaseClass, DataType>::value> * Enable = nullptr>
  NetPolymorphic(const DataType & rhs)
  {
    auto & type_info = BaseClass::__s_TypeDatabase.template GetTypeInfo<DataType>();
    m_Ptr = new DataType(rhs);
    m_ClassId = type_info.m_ClassId;
    m_TypeHash = type_info.m_TypeIdHash;
  }

  template <typename DataType, std::enable_if_t<std::is_base_of<BaseClass, DataType>::value> * Enable = nullptr>
  NetPolymorphic(DataType && rhs)
  {
    auto & type_info = BaseClass::__s_TypeDatabase.template GetTypeInfo<DataType>();
    m_Ptr = new DataType(std::move(rhs));
    m_ClassId = type_info.m_ClassId;
    m_TypeHash = type_info.m_TypeIdHash;
  }

  ~NetPolymorphic()
  {
    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(m_ClassId);
    type_info.m_HeapDestroy(m_Ptr);
  }

  NetPolymorphic<BaseClass> & operator = (const NetPolymorphic<BaseClass> & rhs)
  {
    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(m_ClassId);
    type_info.m_HeapDestroy(m_Ptr);

    auto new_type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(rhs.m_ClassId);
    m_Ptr = static_cast<BaseClass *>(new_type_info.m_HeapCreate());
    m_ClassId = rhs.m_ClassId;
    m_TypeHash = rhs.m_TypeHash;

    new_type_info.m_Copy(m_Ptr, rhs.m_Ptr);
    return *this;
  }

  bool operator == (const NetPolymorphic<BaseClass> & rhs) const
  {
    if (m_ClassId != rhs.m_ClassId)
    {
      return false;
    }

    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(m_ClassId);
    return type_info.m_Compare(m_Ptr, rhs.m_Ptr);
  }

  template <class Class, std::enable_if_t<std::is_base_of<BaseClass, Class>::value> * enable = nullptr>
  void SetType()
  {
    auto class_id = BaseClass::__s_TypeDatabase.GetClassId(typeid(Class).hash_code());
    SetType(class_id);
  }

  void SetType(std::size_t class_id)
  {
    if (class_id == m_ClassId)
    {
      return;
    }

    delete m_Ptr;

    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(class_id);

    m_ClassId = class_id;
    m_Ptr = static_cast<BaseClass *>(type_info.m_HeapCreate());
    m_TypeHash = type_info.m_TypeIdHash;
  }

  std::size_t GetTypeHash() const
  {
    return m_TypeHash;
  }

  template <class T>
  T * Get()
  {
    auto base_class_id = BaseClass::__s_TypeDatabase.GetClassId(typeid(T).hash_code());
    if (BaseClass::__s_TypeDatabase.InheritsFrom(m_ClassId, base_class_id))
    {
      return static_cast<T *>(m_Ptr);
    }

    return nullptr;
  }

  template <class T>
  const T * Get() const
  {
    auto base_class_id = BaseClass::__s_TypeDatabase.GetClassId(typeid(T).hash_code());
    if (BaseClass::__s_TypeDatabase.InheritsFrom(m_ClassId, base_class_id))
    {
      return static_cast<const T *>(m_Ptr);
    }

    return nullptr;
  }

  std::size_t GetClassId() const
  {
    return m_ClassId;
  }

  auto & GetTypeInfo() const
  {
    auto & type_info = BaseClass::__s_TypeDatabase.GetTypeInfo(m_ClassId);
    return type_info;
  }

  BaseClass * GetBase()
  {
    return m_Ptr;
  }

  const BaseClass * GetBase() const
  {
    return m_Ptr;
  }

  BaseClass * operator *()
  {
    return m_Ptr;
  }

  const BaseClass * operator *() const
  {
    return m_Ptr;
  }

  BaseClass * operator ->()
  {
    return m_Ptr;
  }

  const BaseClass * operator ->() const
  {
    return m_Ptr;
  }

private:
  gsl::owner<BaseClass *> m_Ptr;
  std::size_t m_ClassId;
  std::size_t m_TypeHash;
};

template <typename T, class NetBitWriter>
struct NetSerializer<NetPolymorphic<T>, NetBitWriter>
{
  void operator()(const NetPolymorphic<T> & val, NetBitWriter & writer)
  {
    auto & type_info = val.GetTypeInfo();
    writer.WriteBits(val.GetClassId(), GetRequiredBits(T::__s_TypeDatabase.GetNumTypes()));
    type_info.m_Serialize(val.GetBase(), writer);
  }
};

template <typename Type, class NetBitWriter>
struct NetSerializerDelta<NetPolymorphic<Type>, NetBitWriter>
{
  bool operator()(const NetPolymorphic<Type> & to, const NetPolymorphic<Type> & from, NetBitWriter & writer)
  {
    if (to.GetClassId() != from.GetClassId())
    {
      writer.WriteBits(1, 1);
      NetSerializeValue(to, writer);
      return true;
    }

    auto cursor = writer.Reserve(1);
    if (to.GetTypeInfo().m_SerializeDelta(*to, *from, writer))
    {
      cursor.WriteBits(0, 1);
      return true;
    }

    writer.RollBack(cursor);
    return false;
  }
};

template <typename T, class NetBitReader>
struct NetDeserializer<NetPolymorphic<T>, NetBitReader>
{
  void operator()(NetPolymorphic<T> & val, NetBitReader & reader)
  {
    uint64_t class_id = reader.ReadUBits(GetRequiredBits(T::__s_TypeDatabase.GetNumTypes()));
    val.SetType(class_id);

    auto & type_info = val.GetTypeInfo();
    type_info.m_Deserialize(val.GetBase(), reader);
  }
};

template <typename Type, class NetBitReader>
struct NetDeserializerDelta<NetPolymorphic<Type>, NetBitReader>
{
  void operator()(NetPolymorphic<Type> & val, NetBitReader & reader)
  {
    auto change_class = reader.ReadUBits(1);
    if (change_class)
    {
      NetDeserializeValue(val, reader);
    }
    else
    {
      auto & type_info = val.GetTypeInfo();
      type_info.m_DeserializeDelta(val.GetBase(), reader);
    }
  }
};

